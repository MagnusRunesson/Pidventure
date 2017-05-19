//
//  main.cpp
//  TinyArcade
//
//  Created by Magnus Runesson on 2015-11-27.
//  Copyright © 2015 Magnus Runesson. All rights reserved.
//

#include <iostream>
#include "SDL2/SDL.h"
#include "Engine/Graphics/Image.h"
#include "Engine/IO/Joypad.h"
#include "Engine/Graphics/Screen.h"
#include "Engine/Core/Types.h"
#include "Engine/Core/BitHelpers.h"
#include "Engine/Core/Timer.h"
#include "Engine/Core/Memory.h"

#include "Engine/Audio/AudioHandler.h"

//
//
//
extern void game_setup();
extern void game_loop();
extern void game_exit();
extern void game_debugTrigger( int );

SDL_Window* window;
SDL_Surface* screenSurface;

const int SCREEN_PIXELSIZE = 9;

float* screenBuffer;

void audioInit( int _frequency );
void audioExit();


/*
const Uint16 COLORMASK_16_R = COLORWIDTH_16_R << COLORSHIFT_16_R;
const Uint16 COLORMASK_16_G = COLORWIDTH_16_R << COLORSHIFT_16_G;
const Uint16 COLORMASK_16_B = COLORWIDTH_16_R << COLORSHIFT_16_B;

const Uint16 shiftMask[] =
{
	0xFFFF,	// Shift 0 bits
	0xF7DE, // Shift 1 bit
	0xE79C, // Shift 2 bits
	0xC718, // Shift 3 bits
	0x8610, // Shift 4 bits
};
 */

int gx;
int gy;
int gKeyDPadBuff;

extern uint8 gKeyBuff;

SDL_AudioDeviceID dev;

bool init()
{
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
	
	screenBuffer = new float[ SCREEN_WIDTH * SCREEN_HEIGHT*4 ];
	printf("screen buffer=0x%016llx\n", (unsigned long long)screenBuffer );

	/*
	int s = SCREEN_WIDTH * SCREEN_HEIGHT;
	int i;
	for( i=0; i<s; i++ )
		screenBuffer[ i ] = i*10;
	 */
	
	gKeyBuff = 0;
	gKeyDPadBuff = 0;
	gx = 10;
	gy = 10;
	
	timerInit();
	padInit();
	
	dev = NULL;
	audioInit( 11025 );
	
	return true;
}

void audioInit( int _frequency )
{
	if( dev == 0 )
		SDL_CloseAudioDevice( dev );
	
	SDL_AudioSpec want, have;
	
	SDL_memset(&want, 0, sizeof(want)); /* or SDL_zero(want) */
	want.freq = _frequency;
	want.format = AUDIO_S8;
	want.channels = 1;
	want.samples = 10;
	want.callback = Audio_Handler_SDL;
	
	dev = SDL_OpenAudioDevice( NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE );
	
	SDL_PauseAudioDevice( dev, 0 );
}

void exit()
{
	game_exit();
	
	delete[] screenBuffer;
	
	SDL_Quit();
}

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

void blit_depthBufferToSDL()
{
	static float alphaLow = 0.0f;
	static float alphaHigh = 1.0f;
	
	uint32* pixels = (uint32*)screenSurface->pixels;
	
	int scw = SCREEN_WIDTH*SCREEN_PIXELSIZE;
	//int sch = SCREEN_HEIGHT*SCREEN_PIXELSIZE;
	
	float newAlphaLow = 1000.0f;
	float newAlphaHigh = -1000.0f;
	
	int x, y;
	int zx, zy;
	for( y=0; y<SCREEN_HEIGHT; y++ )
	{
		for( x=0; x<SCREEN_WIDTH; x++ )
		{
			int scrofs = (y*SCREEN_WIDTH)+x;
			
			float a = screenBuffer[ (scrofs*4) + 3];
			
			if( a < newAlphaLow ) newAlphaLow = a;
			if( a > newAlphaHigh ) newAlphaHigh = a;
			float ac = ((a-alphaLow) / (alphaHigh-alphaLow));
			if( ac < 0.0f ) ac = 0.0f;
			if( ac > 1.0f ) ac = 1.0f;
			
			uint8 br = (uint8)(ac*255.0f);
			uint8 bg = (uint8)(ac*255.0f);
			uint8 bb = (uint8)(ac*255.0f);
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
	
	alphaLow = newAlphaLow;
	alphaHigh = newAlphaHigh;
	
	SDL_UpdateWindowSurface( window );
	//SDL_Delay( 15 );
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

const int SDL_KEYCODE_ACTION_UL		= SDLK_a;
const int SDL_KEYCODE_ACTION_UM		= SDLK_s;
const int SDL_KEYCODE_ACTION_UR		= SDLK_d;
const int SDL_KEYCODE_ACTION_LL		= SDLK_z;
const int SDL_KEYCODE_ACTION_LM		= SDLK_x;
const int SDL_KEYCODE_ACTION_LR		= SDLK_c;
const int SDL_KEYCODE_PGM_L			= SDLK_SPACE;
const int SDL_KEYCODE_PGM_R			= SDLK_RETURN;

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
					
				case SDL_KEYCODE_ACTION_UL:		SetBit( gKeyBuff, PAD_KEYMASK_ACTION_UL );			break;
				case SDL_KEYCODE_ACTION_UM:		SetBit( gKeyBuff, PAD_KEYMASK_ACTION_UM );			break;
				case SDL_KEYCODE_ACTION_UR:		SetBit( gKeyBuff, PAD_KEYMASK_ACTION_UR );			break;
				case SDL_KEYCODE_ACTION_LL:		SetBit( gKeyBuff, PAD_KEYMASK_ACTION_LL );			break;
				case SDL_KEYCODE_ACTION_LM:		SetBit( gKeyBuff, PAD_KEYMASK_ACTION_LM );			break;
				case SDL_KEYCODE_ACTION_LR:		SetBit( gKeyBuff, PAD_KEYMASK_ACTION_LR );			break;
				case SDL_KEYCODE_PGM_L:			SetBit( gKeyBuff, PAD_KEYMASK_PGM_L );				break;
				case SDL_KEYCODE_PGM_R:			SetBit( gKeyBuff, PAD_KEYMASK_PGM_R );				break;
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
				case SDL_KEYCODE_ACTION_UL:		ClrBit( gKeyBuff, PAD_KEYMASK_ACTION_UL );			break;
				case SDL_KEYCODE_ACTION_UM:		ClrBit( gKeyBuff, PAD_KEYMASK_ACTION_UM );			break;
				case SDL_KEYCODE_ACTION_UR:		ClrBit( gKeyBuff, PAD_KEYMASK_ACTION_UR );			break;
				case SDL_KEYCODE_ACTION_LL:		ClrBit( gKeyBuff, PAD_KEYMASK_ACTION_LL );			break;
				case SDL_KEYCODE_ACTION_LM:		ClrBit( gKeyBuff, PAD_KEYMASK_ACTION_LM );			break;
				case SDL_KEYCODE_ACTION_LR:		ClrBit( gKeyBuff, PAD_KEYMASK_ACTION_LR );			break;
				case SDL_KEYCODE_PGM_L:			ClrBit( gKeyBuff, PAD_KEYMASK_PGM_L );				break;
				case SDL_KEYCODE_PGM_R:			ClrBit( gKeyBuff, PAD_KEYMASK_PGM_R );				break;
				case SDL_KEYCODE_DPAD_UP:		ClrBit( gKeyDPadBuff, PAD_KEYMASK_DPAD_UP );		break;
				case SDL_KEYCODE_DPAD_DOWN:		ClrBit( gKeyDPadBuff, PAD_KEYMASK_DPAD_DOWN );		break;
				case SDL_KEYCODE_DPAD_LEFT:		ClrBit( gKeyDPadBuff, PAD_KEYMASK_DPAD_LEFT );		break;
				case SDL_KEYCODE_DPAD_RIGHT:	ClrBit( gKeyDPadBuff, PAD_KEYMASK_DPAD_RIGHT );		break;
			}
		}
	}

	return quit;
}

bool update()
{
	bool quit = DoSDL();

	padUpdate();
	game_loop();
	
	blit_screenBufferToSDL();
	
	return !quit;
}

int main(int argc, const char * argv[])
{
	dev = NULL;
	memInit();
	
	if( init() == true )
	{
		game_setup();	// Call Arduino like code
		
		// Main loop woot!
		while( update());
	}
	
	exit();
	memExit();
	
    return 0;
}
