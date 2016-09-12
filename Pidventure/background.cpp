//
//  background.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-08.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include <math.h>
#include "Engine/Graphics/Image.h"
#include "Engine/Graphics/Sprite.h"
#include "Engine/Graphics/SpriteRenderer.h"
#include "Engine/Graphics/Screen.h"
#include "Engine/Core/debug.h"
#include "Pidventure/background.h"

Image* bgImage[7];
Sprite* bgSprite[7];
float heights[7] = {
	28.0f,
	28.0f,
	25.0f,
	25.0f,
	22.0f,
	22.0f,
	0.0f
};

void bgInitSprite( int _i )
{
	Sprite* sprite = spriteRenderer.AllocateSprite( bgImage[ _i ]);
	sprite->x = 0.0f;
	sprite->y = heights[ _i ];
	sprite->SetSort( -100.0f - _i );
	bgSprite[ _i ] = sprite;
}

void bgInit()
{
	bgImage[ 0 ] = imageLoad( "sprite_highlands_bg0a" );
	bgImage[ 1 ] = imageLoad( "sprite_highlands_bg0b" );
	bgImage[ 2 ] = imageLoad( "sprite_highlands_bg1a" );
	bgImage[ 3 ] = imageLoad( "sprite_highlands_bg1b" );
	bgImage[ 4 ] = imageLoad( "sprite_highlands_bg2a" );
	bgImage[ 5 ] = imageLoad( "sprite_highlands_bg2b" );
	bgImage[ 6 ] = imageLoad( "sprite_highlands_bg3" );
	
	int i;
	for(i=0; i<7; i++)
		bgInitSprite( i );
}

void coolwrap(float _in, int _layerBase)
{
	int ixa = (int)_in;
	int ixb = ixa+96;
	while(ixa < -96) ixa += 192;
	while(ixb < -96) ixb += 192;
	
	bgSprite[ _layerBase + 0 ]->x = ixa;
	bgSprite[ _layerBase + 1 ]->x = ixb;
}

void bgSetCameraPosition( float _x, float _y )
{
	coolwrap(-_x/5.0f, 0);
	coolwrap(-_x/10.0f, 2);
	coolwrap(-_x/15.0f, 4);
}

void bgExit()
{
	int i;
	for( i=0; i<7; i++ )
	{
		spriteRenderer.FreeSprite( bgSprite[ i ]);
		imageUnload( bgImage[ i ]);
	}
}
