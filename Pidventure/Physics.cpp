//
//  Physics.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-13.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include "Engine/Graphics/Image.h"
#include "Engine/Graphics/SpriteRenderer.h"
#include "Pidventure/Physics.h"

Image* physCollisionMap;
Sprite* physCollisionDebugSprite;
void physInit()
{
	physCollisionMap = imageLoad( "sprite_collisiontest" );
	/*
	physCollisionDebugSprite = spriteRenderer.AllocateSprite( physCollisionMap );
	physCollisionDebugSprite->x = 0.0f;
	physCollisionDebugSprite->y = 0.0f;
	physCollisionDebugSprite->SetSort( 200.0f );
	 */
}

bool physIsCollision(int _x, int _y)
{
	if(_x < 0) return false;
	if(_x >= physCollisionMap->w) return false;
	if(_y < 0) return false;
	if(_y >= physCollisionMap->h) return false;
	int rdofs = (_y*physCollisionMap->w)+_x;
	float r = physCollisionMap->pixels[ rdofs*4 + 0 ];	// Red channel is collision
	return r > 0.5f;
}
