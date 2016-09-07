//
//  Sprite.cpp
//  TinyArcade
//
//  Created by Magnus Runesson on 2015-12-27.
//  Copyright © 2015 Magnus Runesson. All rights reserved.
//

#include <stdlib.h>
#include "Engine/Graphics/SpriteRenderer.h"
#include "Engine/Graphics/Image.h"
#include "Engine/Core/BitHelpers.h"
#include "Engine/Core/Debug.h"

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

void Sprite::SetSort( sint8 _newSort )
{
	sort = _newSort;
	spriteRenderer.SortAllSprites();
}
