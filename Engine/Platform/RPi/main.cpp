//
//  main.cpp
//  TinyArcade
//
//  Created by Magnus Runesson on 2015-11-27.
//  Copyright © 2015 Magnus Runesson. All rights reserved.
//

#include <iostream>
#include <thread>
//#include "SDL2/SDL.h"
#include "Engine/Graphics/Image.h"
#include "Engine/IO/Joypad.h"
#include "Engine/Graphics/Screen.h"
#include "Engine/Core/Types.h"
#include "Engine/Core/BitHelpers.h"
#include "Engine/Core/Timer.h"
#include "Engine/Audio/AudioHandler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>

#include "bcm_host.h"

#include "GLES/gl.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

#include "Engine/Platform/RPi/cube_texture_and_coords.h"
#include "audioplay.h"

//#define PATH "/opt/vc/src/hello_pi/hello_triangle/"

#define IMAGE_SIZE 128

#ifndef M_PI
   #define M_PI 3.141592654
#endif

unsigned char screenBufferUnsignedChar[ IMAGE_SIZE*IMAGE_SIZE*4 ];

typedef struct
{
   uint32_t screen_width;
   uint32_t screen_height;
// OpenGL|ES objects
   DISPMANX_DISPLAY_HANDLE_T dispman_display;
   DISPMANX_ELEMENT_HANDLE_T dispman_element;
   EGLDisplay display;
   EGLSurface surface;
   EGLContext context;
   GLuint tex[6];
// model rotation vector and direction
   GLfloat rot_angle_x_inc;
   GLfloat rot_angle_y_inc;
   GLfloat rot_angle_z_inc;
// current model rotation angles
   GLfloat rot_angle_x;
   GLfloat rot_angle_y;
   GLfloat rot_angle_z;
// current distance from camera
   GLfloat distance;
   GLfloat distance_inc;
// pointers to texture buffers
   char *tex_buf1;
   char *tex_buf2;
   char *tex_buf3;
} CUBE_STATE_T;

static void init_ogl(CUBE_STATE_T *state);
static void init_model_proj(CUBE_STATE_T *state);
static void reset_model(CUBE_STATE_T *state);
static GLfloat inc_and_wrap_angle(GLfloat angle, GLfloat angle_inc);
static GLfloat inc_and_clip_distance(GLfloat distance, GLfloat distance_inc);
static void redraw_scene(CUBE_STATE_T *state);
static void update_model(CUBE_STATE_T *state);
static void init_textures(CUBE_STATE_T *state);
static void load_tex_images(CUBE_STATE_T *state);
static void exit_func(void);
static volatile int terminate;
static CUBE_STATE_T _state, *state=&_state;


/***********************************************************
 * Name: init_ogl
 *
 * Arguments:
 *       CUBE_STATE_T *state - holds OGLES model info
 *
 * Description: Sets the display, OpenGL|ES context and screen stuff
 *
 * Returns: void
 *
 ***********************************************************/
static void init_ogl(CUBE_STATE_T *state)
{
   int32_t success = 0;
   EGLBoolean result;
   EGLint num_config;

   static EGL_DISPMANX_WINDOW_T nativewindow;

   DISPMANX_UPDATE_HANDLE_T dispman_update;
   VC_RECT_T dst_rect;
   VC_RECT_T src_rect;

   static const EGLint attribute_list[] =
   {
      EGL_RED_SIZE, 8,
      EGL_GREEN_SIZE, 8,
      EGL_BLUE_SIZE, 8,
      EGL_ALPHA_SIZE, 8,
      EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
      EGL_NONE
   };
   
   EGLConfig config;

   // get an EGL display connection
   state->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
   assert(state->display!=EGL_NO_DISPLAY);

   // initialize the EGL display connection
   result = eglInitialize(state->display, NULL, NULL);
   assert(EGL_FALSE != result);

   // get an appropriate EGL frame buffer configuration
   result = eglChooseConfig(state->display, attribute_list, &config, 1, &num_config);
   assert(EGL_FALSE != result);

   // create an EGL rendering context
   state->context = eglCreateContext(state->display, config, EGL_NO_CONTEXT, NULL);
   assert(state->context!=EGL_NO_CONTEXT);

   // create an EGL window surface
   success = graphics_get_display_size(0 /* LCD */, &state->screen_width, &state->screen_height);
   assert( success >= 0 );

   dst_rect.x = 0;
   dst_rect.y = 0;
   dst_rect.width = state->screen_width;
   dst_rect.height = state->screen_height;
      
   src_rect.x = 0;
   src_rect.y = 0;
   src_rect.width = state->screen_width << 16;
   src_rect.height = state->screen_height << 16;        

   state->dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
   dispman_update = vc_dispmanx_update_start( 0 );
         
   state->dispman_element = vc_dispmanx_element_add ( dispman_update, state->dispman_display,
      0/*layer*/, &dst_rect, 0/*src*/,
      &src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0/*clamp*/, (DISPMANX_TRANSFORM_T)0/*transform*/);
      
   nativewindow.element = state->dispman_element;
   nativewindow.width = state->screen_width;
   nativewindow.height = state->screen_height;
   vc_dispmanx_update_submit_sync( dispman_update );
      
   state->surface = eglCreateWindowSurface( state->display, config, &nativewindow, NULL );
   assert(state->surface != EGL_NO_SURFACE);

   // connect the context to the surface
   result = eglMakeCurrent(state->display, state->surface, state->surface, state->context);
   assert(EGL_FALSE != result);

   // Set background color and clear buffers
   glClearColor(0.f, 0.f, 0.f, 1.0f);

   // Enable back face culling.
   glEnable(GL_CULL_FACE);

   glMatrixMode(GL_MODELVIEW);
}

/***********************************************************
 * Name: init_model_proj
 *
 * Arguments:
 *       CUBE_STATE_T *state - holds OGLES model info
 *
 * Description: Sets the OpenGL|ES model to default values
 *
 * Returns: void
 *
 ***********************************************************/
static void init_model_proj(CUBE_STATE_T *state)
{
   float nearp = 1.0f;
   float farp = 500.0f;
   float hht;
   float hwd;

   glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

   glViewport(0, 0, (GLsizei)state->screen_width, (GLsizei)state->screen_height);
      
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   hht = nearp * (float)tan(45.0 / 2.0 / 180.0 * M_PI);
   hwd = hht * (float)state->screen_width / (float)state->screen_height;

   glFrustumf(-hwd, hwd, -hht, hht, nearp, farp);
   
   glEnableClientState( GL_VERTEX_ARRAY );
   glVertexPointer( 3, GL_BYTE, 0, quadx );

   reset_model(state);
}

/***********************************************************
 * Name: reset_model
 *
 * Arguments:
 *       CUBE_STATE_T *state - holds OGLES model info
 *
 * Description: Resets the Model projection and rotation direction
 *
 * Returns: void
 *
 ***********************************************************/
static void reset_model(CUBE_STATE_T *state)
{
   // reset model position
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glTranslatef(0.f, 0.f, -20.f);

   // reset model rotation
   state->rot_angle_x = 45.f; state->rot_angle_y = 30.f; state->rot_angle_z = 0.f;
   state->rot_angle_x_inc = 0.5f; state->rot_angle_y_inc = 0.5f; state->rot_angle_z_inc = 0.f;
   //state->rot_angle_x_inc = 0.f; state->rot_angle_y_inc = 0.f; state->rot_angle_z_inc = 0.f;
   state->distance = 20.f;
}

/***********************************************************
 * Name: update_model
 *
 * Arguments:
 *       CUBE_STATE_T *state - holds OGLES model info
 *
 * Description: Updates model projection to current position/rotation
 *
 * Returns: void
 *
 ***********************************************************/
static void update_model(CUBE_STATE_T *state)
{
   // update position
   state->rot_angle_x = inc_and_wrap_angle(state->rot_angle_x, state->rot_angle_x_inc);
   state->rot_angle_y = inc_and_wrap_angle(state->rot_angle_y, state->rot_angle_y_inc);
   state->rot_angle_z = inc_and_wrap_angle(state->rot_angle_z, state->rot_angle_z_inc);
   state->distance    = inc_and_clip_distance(state->distance, state->distance_inc);

   glLoadIdentity();
   // move camera back to see the cube
   glTranslatef(0.f, 0.f, -state->distance);

   // Rotate model to new position
   //glRotatef(state->rot_angle_y, 0.f, 1.f, 0.f );
   //glRotatef(state->rot_angle_x, 1.f, 0.f, 0.f);
   //glRotatef(state->rot_angle_y, 0.f, 1.f, 0.f);
   //glRotatef(state->rot_angle_z, 0.f, 0.f, 1.f);
}

/***********************************************************
 * Name: inc_and_wrap_angle
 *
 * Arguments:
 *       GLfloat angle     current angle
 *       GLfloat angle_inc angle increment
 *
 * Description:   Increments or decrements angle by angle_inc degrees
 *                Wraps to 0 at 360 deg.
 *
 * Returns: new value of angle
 *
 ***********************************************************/
static GLfloat inc_and_wrap_angle(GLfloat angle, GLfloat angle_inc)
{
   angle += angle_inc;

   if (angle >= 360.0)
      angle -= 360.f;
   else if (angle <=0)
      angle += 360.f;

   return angle;
}

/***********************************************************
 * Name: inc_and_clip_distance
 *
 * Arguments:
 *       GLfloat distance     current distance
 *       GLfloat distance_inc distance increment
 *
 * Description:   Increments or decrements distance by distance_inc units
 *                Clips to range
 *
 * Returns: new value of angle
 *
 ***********************************************************/
static GLfloat inc_and_clip_distance(GLfloat distance, GLfloat distance_inc)
{
   distance += distance_inc;

   if (distance >= 120.0f)
      distance = 120.f;
   else if (distance <= 40.0f)
      distance = 40.0f;

   return distance;
}

/***********************************************************
 * Name: redraw_scene
 *
 * Arguments:
 *       CUBE_STATE_T *state - holds OGLES model info
 *
 * Description:   Draws the model and calls eglSwapBuffers
 *                to render to screen
 *
 * Returns: void
 *
 ***********************************************************/
static void redraw_scene(CUBE_STATE_T *state)
{
   // Start with a clear screen
   glClear( GL_COLOR_BUFFER_BIT );

   // Draw first (front) face:
   // Bind texture surface to current vertices
   glBindTexture(GL_TEXTURE_2D, state->tex[0]);
   //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, IMAGE_SIZE, IMAGE_SIZE, 0, GL_RGBA, GL_FLOAT, screenBuffer);

   int x,y;
   for(y=0; y<74; y++ )
   {
   		for(x=0; x<96; x++ )
   		{
   			int rdofs = ((y*SCREEN_WIDTH)+x)*4;
   			float sr = screenBuffer[ rdofs+0 ];
   			float sg = screenBuffer[ rdofs+1 ];
   			float sb = screenBuffer[ rdofs+2 ];
   			unsigned char r = (unsigned char)(sr*255.0f);
   			unsigned char g = (unsigned char)(sg*255.0f);
   			unsigned char b = (unsigned char)(sb*255.0f);
   			unsigned char a = 255;

   			int wrofs = ((y*IMAGE_SIZE)+x)*4;
   			screenBufferUnsignedChar[ wrofs+0 ] = r;
   			screenBufferUnsignedChar[ wrofs+1 ] = g;
   			screenBufferUnsignedChar[ wrofs+2 ] = b;
   			screenBufferUnsignedChar[ wrofs+3 ] = a;
   		}
   }

   glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, IMAGE_SIZE, IMAGE_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, screenBufferUnsignedChar );

   // Need to rotate textures - do this by rotating each cube face
   glRotatef(270.f, 0.f, 0.f, 1.f ); // front face normal along z axis

   // draw first 4 vertices
   glDrawArrays( GL_TRIANGLE_STRIP, 0, 4);

/*
   // same pattern for other 5 faces - rotation chosen to make image orientation 'nice'
   glBindTexture(GL_TEXTURE_2D, state->tex[1]);
   glRotatef(90.f, 0.f, 0.f, 1.f ); // back face normal along z axis
   glDrawArrays( GL_TRIANGLE_STRIP, 4, 4);

   glBindTexture(GL_TEXTURE_2D, state->tex[2]);
   glRotatef(90.f, 1.f, 0.f, 0.f ); // left face normal along x axis
   glDrawArrays( GL_TRIANGLE_STRIP, 8, 4);

   glBindTexture(GL_TEXTURE_2D, state->tex[3]);
   glRotatef(90.f, 1.f, 0.f, 0.f ); // right face normal along x axis
   glDrawArrays( GL_TRIANGLE_STRIP, 12, 4);

   glBindTexture(GL_TEXTURE_2D, state->tex[4]);
   glRotatef(270.f, 0.f, 1.f, 0.f ); // top face normal along y axis
   glDrawArrays( GL_TRIANGLE_STRIP, 16, 4);

   glBindTexture(GL_TEXTURE_2D, state->tex[5]);
   glRotatef(90.f, 0.f, 1.f, 0.f ); // bottom face normal along y axis
   glDrawArrays( GL_TRIANGLE_STRIP, 20, 4);
   */

   eglSwapBuffers(state->display, state->surface);
}

/***********************************************************
 * Name: init_textures
 *
 * Arguments:
 *       CUBE_STATE_T *state - holds OGLES model info
 *
 * Description:   Initialise OGL|ES texture surfaces to use image
 *                buffers
 *
 * Returns: void
 *
 ***********************************************************/
static void init_textures(CUBE_STATE_T *state)
{
   // load three texture buffers but use them on six OGL|ES texture surfaces
   /*load_tex_images(state);
   glGenTextures(6, &state->tex[0]);

   // setup first texture
   glBindTexture(GL_TEXTURE_2D, state->tex[0]);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMAGE_SIZE, IMAGE_SIZE, 0,
                GL_RGB, GL_UNSIGNED_BYTE, state->tex_buf1);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);

   // setup second texture - reuse first image
   glBindTexture(GL_TEXTURE_2D, state->tex[1]);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMAGE_SIZE, IMAGE_SIZE, 0,
                GL_RGB, GL_UNSIGNED_BYTE, state->tex_buf1);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);

   // third texture
   glBindTexture(GL_TEXTURE_2D, state->tex[2]);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMAGE_SIZE, IMAGE_SIZE, 0,
                GL_RGB, GL_UNSIGNED_BYTE, state->tex_buf2);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);

   // fourth texture  - reuse second image
   glBindTexture(GL_TEXTURE_2D, state->tex[3]);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMAGE_SIZE, IMAGE_SIZE, 0,
                GL_RGB, GL_UNSIGNED_BYTE, state->tex_buf2);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);

   //fifth texture
   glBindTexture(GL_TEXTURE_2D, state->tex[4]);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMAGE_SIZE, IMAGE_SIZE, 0,
                GL_RGB, GL_UNSIGNED_BYTE, state->tex_buf3);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);

   // sixth texture  - reuse third image
   glBindTexture(GL_TEXTURE_2D, state->tex[5]);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMAGE_SIZE, IMAGE_SIZE, 0,
                GL_RGB, GL_UNSIGNED_BYTE, state->tex_buf3);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);
   */

   glGenTextures(1, &state->tex[0]);
   printf("gen textures - error: 0x%08x\n", glGetError());
   glBindTexture(GL_TEXTURE_2D, state->tex[0]);
   printf("bind texture - error: 0x%08x\n", glGetError());
   int x,y;
   for(y=0; y<74; y++ )
   {
   		for(x=0; x<96; x++ )
   		{
   			unsigned char r = (y*3);
   			unsigned char g = (x*3);
   			unsigned char b = r ^ g;
   			unsigned char a = 255;

   			int ofs = ((y*IMAGE_SIZE)+x)*4;
   			screenBufferUnsignedChar[ ofs+0 ] = r;
   			screenBufferUnsignedChar[ ofs+1 ] = g;
   			screenBufferUnsignedChar[ ofs+2 ] = b;
   			screenBufferUnsignedChar[ ofs+3 ] = a;
   		}
   }

   glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, IMAGE_SIZE, IMAGE_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, screenBufferUnsignedChar );
   printf("tex image - error: 0x%08x\n", glGetError());
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);

   // setup overall texture environment
   glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   
   glEnable(GL_TEXTURE_2D);
}

/***********************************************************
 * Name: load_tex_images
 *
 * Arguments:
 *       void
 *
 * Description: Loads three raw images to use as textures on faces
 *
 * Returns: void
 *
 ***********************************************************/
 /*
static void load_tex_images(CUBE_STATE_T *state)
{
   FILE *tex_file1 = NULL, *tex_file2=NULL, *tex_file3 = NULL;
   int bytes_read, image_sz = IMAGE_SIZE*IMAGE_SIZE*3;

   state->tex_buf1 = malloc(image_sz);
   state->tex_buf2 = malloc(image_sz);
   state->tex_buf3 = malloc(image_sz);

   tex_file1 = fopen(PATH "Lucca_128_128.raw", "rb");
   if (tex_file1 && state->tex_buf1)
   {
      bytes_read=fread(state->tex_buf1, 1, image_sz, tex_file1);
      assert(bytes_read == image_sz);  // some problem with file?
      fclose(tex_file1);
   }

   tex_file2 = fopen(PATH "Djenne_128_128.raw", "rb");
   if (tex_file2 && state->tex_buf2)
   {
      bytes_read=fread(state->tex_buf2, 1, image_sz, tex_file2);
      assert(bytes_read == image_sz);  // some problem with file?
      fclose(tex_file2);      
   }

   tex_file3 = fopen(PATH "Gaudi_128_128.raw", "rb");
   if (tex_file3 && state->tex_buf3)
   {
      bytes_read=fread(state->tex_buf3, 1, image_sz, tex_file3);
      assert(bytes_read == image_sz);  // some problem with file?
      fclose(tex_file3);
   }   
}
*/

//------------------------------------------------------------------------------

static void exit_func(void)
// Function to be passed to atexit().
{
   DISPMANX_UPDATE_HANDLE_T dispman_update;
   int s;
   // clear screen
   glClear( GL_COLOR_BUFFER_BIT );
   eglSwapBuffers(state->display, state->surface);

   glDeleteTextures(1, state->tex);
   eglDestroySurface( state->display, state->surface );

   dispman_update = vc_dispmanx_update_start( 0 );
   s = vc_dispmanx_element_remove(dispman_update, state->dispman_element);
   assert(s == 0);
   vc_dispmanx_update_submit_sync( dispman_update );
   s = vc_dispmanx_display_close(state->dispman_display);
   assert (s == 0);

   // Release OpenGL resources
   eglMakeCurrent( state->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
   eglDestroyContext( state->display, state->context );
   eglTerminate( state->display );

/*
   // release texture buffers
   free(state->tex_buf1);
   free(state->tex_buf2);
   free(state->tex_buf3);
   */

   printf("\ncube closed\n");
} // exit_func()

//==============================================================================






//
//
//
extern void game_setup();
extern void game_loop();
extern void game_exit();
extern void game_debugTrigger( int );

//SDL_Window* window;
//SDL_Surface* screenSurface;

const int SCREEN_PIXELSIZE = 9;

float* screenBuffer;

void audioInit( int _frequency );
void audioExit();


int gx;
int gy;
int gKeyDPadBuff;

extern uint8 gKeyBuff;

//SDL_AudioDeviceID dev;

bool init()
{

	/*
	bool success = true;
	
	printf("init!\n");
	
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO ) < 0 )
	{
		printf("Failed to init SDL. Error: %s\n", SDL_GetError());
		success = false;
	} else
	{
		window = SDL_CreateWindow("Pidventure", 600, 100, SCREEN_WIDTH*SCREEN_PIXELSIZE, SCREEN_HEIGHT*SCREEN_PIXELSIZE, SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_OPENGL );
		if( window == NULL )
		{
			printf("Failed to create window. Error: %s\n", SDL_GetError());
			success = false;
		} else
		{
			printf("Happy fun times!\n");
			screenSurface = SDL_GetWindowSurface( window );
		}
	}
	
	SDL_GL_SetSwapInterval(1);
	printf( "screen surface=0x%016llx\n", (unsigned long long)screenSurface );
	printf( "screen surface pixels=0x%016llx\n", (unsigned long long)(screenSurface->pixels) );
	printf( "screen surface width=%i, height=%i, bpp=%i, bytespp=%i\n", screenSurface->w, screenSurface->h, screenSurface->format->BitsPerPixel, screenSurface->format->BytesPerPixel );
	*/
	
	screenBuffer = new float[ SCREEN_WIDTH * SCREEN_HEIGHT*4 ];
	printf("screen buffer=0x%016llx\n", (unsigned long long)screenBuffer );

	/*
	int s = SCREEN_WIDTH * SCREEN_HEIGHT;
	int i;
	for( i=0; i<s; i++ )
		screenBuffer[ i ] = i*10;
	 * /
	 */
	
   bcm_host_init();

   // Clear application state
   memset( state, 0, sizeof( *state ) );
      
   // Start OGLES
   init_ogl(state);

   // Setup the model world
   init_model_proj(state);

   // initialise the OGLES texture(s)
   init_textures(state);


	gKeyBuff = 0;
	gKeyDPadBuff = 0;
	gx = 10;
	gy = 10;
	
	timerInit();
	padInit();
	
	//dev = NULL;
	audioInit( 11025 );
	
	return true;
}

void audioInit( int _frequency )
{
	/*
	if( dev == 0 )
		SDL_CloseAudioDevice( dev );
	
	SDL_AudioSpec want, have;
	
	SDL_memset(&want, 0, sizeof(want)); /* or SDL_zero(want) * /
	want.freq = _frequency;
	want.format = AUDIO_S8;
	want.channels = 1;
	want.samples = 10;
	want.callback = Audio_Handler_SDL;
	
	dev = SDL_OpenAudioDevice( NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE );
	
	SDL_PauseAudioDevice( dev, 0 );
	*/
}

void exit()
{
	game_exit();
	
	exit_func();

	delete[] screenBuffer;
	
	//SDL_Quit();
}

/*
Uint32 Doo( Uint16 _src, int _shiftDown, int _width, int _shiftUp )
{
	Uint32 ret = _src >> _shiftDown;
	ret &= _width;
	ret <<= _shiftUp;
	return ret;
}

Uint32 Conv16to32( Uint16 src )
{
	Uint32 ret = 0;
	
	Uint32 r = Doo( src, COLORSHIFT_16_R, COLORWIDTH_16_R, 16+3 );
	Uint32 g = Doo( src, COLORSHIFT_16_G, COLORWIDTH_16_G, 8+2 );
	Uint32 b = Doo( src, COLORSHIFT_16_B, COLORWIDTH_16_B, 0+3 );

	ret = r | g | b;
	
	return ret;
}

void blit_screenBufferToSDL()
{
	uint32* pixels = (uint32*)screenSurface->pixels;
	
	int scw = SCREEN_WIDTH*SCREEN_PIXELSIZE;
	//int sch = SCREEN_HEIGHT*SCREEN_PIXELSIZE;

	int x, y;
	int zx, zy;
	for( y=0; y<SCREEN_HEIGHT; y++ )
	{
		for( x=0; x<SCREEN_WIDTH; x++ )
		{
			int scrofs = (y*SCREEN_WIDTH)+x;

			float r = screenBuffer[ (scrofs*4) + 0];
			float g = screenBuffer[ (scrofs*4) + 1];
			float b = screenBuffer[ (scrofs*4) + 2];
			uint8 br = (uint8)(r*255.0f);
			uint8 bg = (uint8)(g*255.0f);
			uint8 bb = (uint8)(b*255.0f);
			uint32 c = (br<<16) + (bg<<8) + bb;
			
			for( zy=0; zy<SCREEN_PIXELSIZE; zy++ )
			{
				for( zx=0; zx<SCREEN_PIXELSIZE; zx++ )
				{
					int wrx = (x*SCREEN_PIXELSIZE) + zx;
					int wry = (y*SCREEN_PIXELSIZE) + zy;
					int wrofs = (wry*scw) + wrx;
					
					pixels[ wrofs ] = c;
				}
			}
		}
	}

	SDL_UpdateWindowSurface( window );
	//SDL_Delay( 15 );
}

const int SDL_KEYCODE_PRIMARY		= SDLK_z;
const int SDL_KEYCODE_SECONDARY		= SDLK_x;
const int SDL_KEYCODE_SELECT		= SDLK_SPACE;
const int SDL_KEYCODE_START			= SDLK_RETURN;

const int SDL_KEYCODE_DPAD_UP		= SDLK_UP;
const int SDL_KEYCODE_DPAD_DOWN		= SDLK_DOWN;
const int SDL_KEYCODE_DPAD_LEFT		= SDLK_LEFT;
const int SDL_KEYCODE_DPAD_RIGHT	= SDLK_RIGHT;

bool DoSDL()
{
	bool quit = false;
	SDL_Event e;
	
	while( SDL_PollEvent( &e ))
	{
		if( e.type == SDL_QUIT )
		{
			quit = true;
		}
		else if( e.type == SDL_KEYDOWN )
		{
			switch( e.key.keysym.sym )
			{
				case SDLK_ESCAPE:
					quit = true;
					break;
					
				case SDL_KEYCODE_PRIMARY:		SetBit( gKeyBuff, PAD_KEYMASK_PRIMARY );			break;
				case SDL_KEYCODE_SECONDARY:		SetBit( gKeyBuff, PAD_KEYMASK_SECONDARY );			break;
				case SDL_KEYCODE_SELECT:		SetBit( gKeyBuff, PAD_KEYMASK_SELECT );				break;
				case SDL_KEYCODE_START:			SetBit( gKeyBuff, PAD_KEYMASK_START );				break;
				case SDL_KEYCODE_DPAD_UP:		SetBit( gKeyDPadBuff, PAD_KEYMASK_DPAD_UP );		break;
				case SDL_KEYCODE_DPAD_DOWN:		SetBit( gKeyDPadBuff, PAD_KEYMASK_DPAD_DOWN );		break;
				case SDL_KEYCODE_DPAD_LEFT:		SetBit( gKeyDPadBuff, PAD_KEYMASK_DPAD_LEFT );		break;
				case SDL_KEYCODE_DPAD_RIGHT:	SetBit( gKeyDPadBuff, PAD_KEYMASK_DPAD_RIGHT );		break;
				case SDLK_0: game_debugTrigger( 0 ); break;
				case SDLK_1: game_debugTrigger( 1 ); break;
				case SDLK_2: game_debugTrigger( 2 ); break;
				case SDLK_3: game_debugTrigger( 3 ); break;
				case SDLK_4: game_debugTrigger( 4 ); break;
				case SDLK_5: game_debugTrigger( 5 ); break;
				case SDLK_6: game_debugTrigger( 6 ); break;
				case SDLK_7: game_debugTrigger( 7 ); break;
				case SDLK_8: game_debugTrigger( 8 ); break;
				case SDLK_9: game_debugTrigger( 9 ); break;
			}
		}
		else if( e.type == SDL_KEYUP )
		{
			switch( e.key.keysym.sym )
			{
				case SDL_KEYCODE_PRIMARY:		ClrBit( gKeyBuff, PAD_KEYMASK_PRIMARY );			break;
				case SDL_KEYCODE_SECONDARY:		ClrBit( gKeyBuff, PAD_KEYMASK_SECONDARY );			break;
				case SDL_KEYCODE_SELECT:		ClrBit( gKeyBuff, PAD_KEYMASK_SELECT );				break;
				case SDL_KEYCODE_START:			ClrBit( gKeyBuff, PAD_KEYMASK_START );				break;
				case SDL_KEYCODE_DPAD_UP:		ClrBit( gKeyDPadBuff, PAD_KEYMASK_DPAD_UP );		break;
				case SDL_KEYCODE_DPAD_DOWN:		ClrBit( gKeyDPadBuff, PAD_KEYMASK_DPAD_DOWN );		break;
				case SDL_KEYCODE_DPAD_LEFT:		ClrBit( gKeyDPadBuff, PAD_KEYMASK_DPAD_LEFT );		break;
				case SDL_KEYCODE_DPAD_RIGHT:	ClrBit( gKeyDPadBuff, PAD_KEYMASK_DPAD_RIGHT );		break;
			}
		}
	}

	return quit;
}
*/

bool update()
{
	//bool quit = DoSDL();
	bool quit = false;

	padUpdate();
	game_loop();
	
	update_model(state);
	redraw_scene(state);

	//blit_screenBufferToSDL();
	
	return !quit;
}


#define N_WAVE          1024    /* dimension of Sinewave[] */
#define PI (1<<16>>1)
#define SIN(x) Sinewave[((x)>>6) & (N_WAVE-1)]
#define COS(x) SIN((x)+(PI>>1))
#define OUT_CHANNELS(num_channels) ((num_channels) > 4 ? 8: (num_channels) > 2 ? 4: (num_channels))
extern short Sinewave[];

#define CTTW_SLEEP_TIME 10
#define MIN_LATENCY_TIME 20
#define BUFFER_SIZE_SAMPLES 1024


static const char *audio_dest[] = {"local", "hdmi"};
void play_api_test(int samplerate, int bitdepth, int nchannels, int dest)
{
   AUDIOPLAY_STATE_T *st;
   int32_t ret;
   unsigned int i, j, n;
   int phase = 0;
   int inc = 256<<16;
   int dinc = 0;
   int buffer_size = (BUFFER_SIZE_SAMPLES * bitdepth * OUT_CHANNELS(nchannels))>>3;

   assert(dest == 0 || dest == 1);

   ret = audioplay_create(&st, samplerate, nchannels, bitdepth, 10, buffer_size);
   assert(ret == 0);

   ret = audioplay_set_dest(st, audio_dest[dest]);
   assert(ret == 0);

   // iterate for 5 seconds worth of packets
   for (n=0; n<((samplerate * 1000)/ BUFFER_SIZE_SAMPLES); n++)
   {
      uint8_t *buf;
      int16_t *p;
      uint32_t latency;

      while((buf = audioplay_get_buffer(st)) == NULL)
         usleep(10*1000);

      p = (int16_t *) buf;

   printf("arne %i / %i\n", n, ((samplerate * 1000)/ BUFFER_SIZE_SAMPLES));

      // fill the buffer
      for (i=0; i<BUFFER_SIZE_SAMPLES; i++)
      {
         int16_t val = SIN(phase);
         phase += inc>>16;
         inc += dinc;
         if (inc>>16 < 512)
            dinc++;
         else
            dinc--;

         for(j=0; j<OUT_CHANNELS(nchannels); j++)
         {
            if (bitdepth == 32)
               *p++ = 0;
            *p++ = val;
         }
      }

      // try and wait for a minimum latency time (in ms) before
      // sending the next packet
      while((latency = audioplay_get_latency(st)) > (samplerate * (MIN_LATENCY_TIME + CTTW_SLEEP_TIME) / 1000))
         usleep(CTTW_SLEEP_TIME*1000);

      ret = audioplay_play_buffer(st, buf, buffer_size);
      assert(ret == 0);
   }

   audioplay_delete(st);
}

void audio_party()
{
   // 0=headphones, 1=hdmi
   int audio_dest = 0;
   // audio sample rate in Hz
   int samplerate = 48000;
   // numnber of audio channels
   int channels = 2;
   // number of bits per sample
   int bitdepth = 16;
   bcm_host_init();

   /*
   if (argc > 1)
      audio_dest = atoi(argv[1]);
   if (argc > 2)
      channels = atoi(argv[2]);
   if (argc > 3)
      samplerate = atoi(argv[3]);
      */

   printf("Outputting audio to %s\n", audio_dest==0 ? "analogue":"hdmi");

   play_api_test(samplerate, bitdepth, channels, audio_dest);
}

int main(int argc, const char * argv[])
{
	//dev = NULL;

   pthread_t audioThread;

   printf("Starting thread\n" );

   pthread_create( &audioThread, NULL, audio_party, NULL );

   printf("Thread started, yay!\n");

	if( init() == true )
	{
		game_setup();	// Call Arduino like code
		
		// Main loop woot!
		while( update());
	}
	
	exit();
	
    return 0;
}
