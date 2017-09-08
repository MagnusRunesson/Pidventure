//
//  Sprite.cpp
//  TinyArcade
//
//  Created by Magnus Runesson on 2015-12-27.
//  Copyright © 2015 Magnus Runesson. All rights reserved.
//

#include "Engine/stdc/stdlib.h"
#include "Engine/Graphics/SpriteRenderer.h"
#include "Engine/Graphics/Image.h"
#include "Engine/Core/BitHelpers.h"
#include "Engine/Core/Debug.h"
#include "Engine/Core/Math.h"

Sprite::Sprite()
{
	Reboot();
}

void Sprite::Reboot()
{
	x = 127.0f;
	y = 127.0f;
	sort = 0.0f;
	flags = 0;
	image = NULL;
	imageDepth = NULL;
	collisionIndex = 0;
	ClrFlags( SPRITE_FLAG_ENABLED );
}

void Sprite::SetRotation( int _rotation )
{
	ClrFlags( SPRITE_FLAG_ROTATE_MASK );
	SetFlags( _rotation << SPRITE_FLAG_ROTATE_BASE );
}

void Sprite::SetFlippedX( bool _flipped )
{
	if( _flipped )
		SetFlags( SPRITE_FLAG_FLIP_X );
	else
		ClrFlags( SPRITE_FLAG_FLIP_X );
}

void Sprite::SetFlippedY( bool _flipped )
{
	if( _flipped )
		SetFlags( SPRITE_FLAG_FLIP_Y );
	else
		ClrFlags( SPRITE_FLAG_FLIP_Y );
}

void Sprite::SetSort( float _newSort )
{
	sort = _newSort;
	spriteRenderer.SortAllSprites();
}

void Sprite::SetDepthImage( const Image* _pDepthImage )
{
	imageDepth = _pDepthImage;
}




//
// Refresh everything needed for rendering sprites, such as refresing their on screen bounds.
//
void Sprite::PreRender()
{
	// Should this sprite even render?
	if( !HasBit( flags, SPRITE_FLAG_ENABLED))
		return;
	
	//
	int rotation = (flags & SPRITE_FLAG_ROTATE_MASK) >> SPRITE_FLAG_ROTATE_BASE;
	int w, h;
	if( rotation & 1 )
	{
		// Rotated 90 or 270 degrees means that width and height should be flipped
		w = image->h;
		h = image->w;
	} else
	{
		w = image->w;
		h = image->h;
	}
	
	int ix = round( x );
	int iy = round( y );
	boundsLeft = ix;
	boundsTop = iy;
	boundsRight = ix+w;
	boundsBottom = iy+h;
}


void Sprite::FrameStart()
{
	if( boundsTop < 0 )
	{
		if( flags & SPRITE_FLAG_FLIP_Y )
		{
			readY = (image->h-1+boundsTop)*image->w;
		}
		else
		{
			readY = (-boundsTop)*image->w;
		}
	}
	else
	{
		if( flags & SPRITE_FLAG_FLIP_Y )
		{
			readY = (image->h-1)*image->w;
		}
		else
		{
			readY = 0;
		}
	}
	
	int xofs = -boundsLeft;
	if( xofs<0) xofs=0;
	
	int rdofs = readY+xofs;
	
	pPixelData = &image->pixels[ rdofs*4 ];
}

void Sprite::NextScanLine()
{
	if( flags & SPRITE_FLAG_FLIP_Y )
		readY -= image->w;
	else
		readY += image->w;
	
	int xofs = -boundsLeft;
	if( xofs<0) xofs=0;
	
	int rdofs = readY+xofs;
	
	pPixelData = &image->pixels[ rdofs*4 ];
}
