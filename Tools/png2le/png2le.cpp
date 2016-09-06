//
//  main.cpp
//  png2c
//
//  Created by Magnus Runesson on 01/12/15.
//  Copyright (c) 2015 Pokewhat. All rights reserved.
//

#include <SDL2/SDL.h>
#include <SDL2_Image/SDL_Image.h>

typedef struct
{
	unsigned int header;	// 32 bit file header
	unsigned short w;		// 16 bit width of image
	unsigned short h;		// 16 bit height of image
	unsigned int flags;		// 32 bit field of flags
	unsigned int padding;	// 32 bit padding
	float rgba[];
} Image;

/*
Image myImage
{
	10, 10, NULL,
};

float pixels[] = {
	0x1111,
	0x2222,
	0xffff
};
*/

void writeHeader( FILE* f, SDL_Surface* image )
{
	unsigned int header = 'PEI0';
	fwrite(&header, 4, 1, f);
	
	unsigned short width = image->w;
	fwrite(&width, 2, 1, f);
	
	unsigned short height = image->h;
	fwrite(&height, 2, 1, f);
	
	unsigned int flags = 0;
	if( SDL_ISPIXELFORMAT_ALPHA( image->format->format ))
		flags |= 1;
	fwrite(&flags, 4, 1, f);

	unsigned int padding = 0;
	fwrite(&padding, 4, 1, f);
}

void writePixels( FILE* f, SDL_Surface* image )
{
	unsigned char* pixels = (unsigned char*)image->pixels;
	int x, y;
	for( y=0; y<image->h; y++ )
	{
		for( x=0; x<image->w; x++ )
		{
			int rofs = ((y*image->w)+x)*4;
			unsigned char bb = pixels[ rofs+0 ];
			unsigned char bg = pixels[ rofs+1 ];
			unsigned char br = pixels[ rofs+2 ];
			unsigned char ba = pixels[ rofs+3 ];

			float fr = ((float)br) / 255.0f;
			float fg = ((float)bg) / 255.0f;
			float fb = ((float)bb) / 255.0f;
			float fa = ((float)ba) / 255.0f;

			fr = fmaxf(0.0f, fminf(fr, 1.0f));
			fg = fmaxf(0.0f, fminf(fg, 1.0f));
			fb = fmaxf(0.0f, fminf(fb, 1.0f));
			fa = fmaxf(0.0f, fminf(fa, 1.0f));
			
			//printf("x=%02i, y=%02i: r=%.5f, g=%.5f, b=%.5f, a=%.5f\n", x, y, fr, fg, fb, fa);
			//printf("x=%2i y=%2i: r=%02x, g=%02x, b=%02x, a=%02x\n", x, y, br, bg, bb, ba);
			
			/*
			if((x==0) && (y==0)) {
				unsigned char* pfest = (unsigned char*)&fr;
				printf("%02x,%02x,%02x,%02x\n", pfest[0], pfest[1], pfest[2], pfest[3]);
			}
			 */
			
			/*fwrite(&br, 1, 1, f);
			fwrite(&bg, 1, 1, f);
			fwrite(&bb, 1, 1, f);
			fwrite(&ba, 1, 1, f);*/
			fwrite(&fr, 4, 1, f);
			fwrite(&fg, 4, 1, f);
			fwrite(&fb, 4, 1, f);
			fwrite(&fa, 4, 1, f);
		}
	}
}

SDL_Surface* LoadImage( char* _fileName )
{
	SDL_Surface* image = IMG_Load( _fileName );
	//printf("Image=0x%016llx\n", (long long)image );

	//bool isAlpha = SDL_ISPIXELFORMAT_ALPHA( image->format->format );
	//bool isIndexed = SDL_ISPIXELFORMAT_INDEXED( image->format->format );
	//printf("w=%i, h=%i, bpp=%i, format=%i, isAlpha=%s, isIndexed=%s\n", image->w, image->h, image->format->BitsPerPixel, image->format->format, isAlpha?"Yes":"No", isIndexed?"Yes":"No" );
	
	return image;
}

FILE* openOutfileLE( char* _baseOutFileName )
{
	char outname_c[ 2048 ];
	sprintf( outname_c, "%s.pei", _baseOutFileName );
	FILE* f = fopen( outname_c, "wb" );
	
	return f;
}

int main( int _numargs, char** _apszArgh )
{
	if( _numargs != 3 )
	{
		printf("Usage error: Program need 2 arguments:\n");
		printf("  png2le <in_file.png> <out_file_base>\n");
		return -1;
	}

	char* pszInFileName = _apszArgh[ 1 ];
	char* pszOutFilenameBase = _apszArgh[ 2 ];
	
	//
	SDL_Surface* image = LoadImage( pszInFileName );

	//
	// Write binary file
	//
	FILE* f = openOutfileLE( pszOutFilenameBase );
	writeHeader( f, image );
	writePixels( f, image );
	fclose( f );
	
	return 0;
}
