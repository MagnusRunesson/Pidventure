//
//  Physics.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-13.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include "Engine/Graphics/Image.h"
#include "Engine/Graphics/SpriteRenderer.h"
#include "Engine/Graphics/TileBank.h"
#include "Engine/Graphics/TileMap.h"
#include "Engine/Graphics/TileRenderer.h"
#include "Pidventure/Physics.h"

Image* physCollisionMap;
Sprite* physCollisionDebugSprite;
static CTileBank* pTileBank;
static CTileMap* pTileMap;
static TileRenderer* pTileRenderer;
float physSample[ 4 ];

void physInit()
{
	//physCollisionMap = imageLoad( "sprite_collisiontest" );
	pTileBank = tilebankLoad( "tilebank_highlands_collision" );
	pTileMap = tilemapLoad( "scene_highlands" );
	pTileRenderer = new TileRenderer( pTileMap, pTileBank );
	/*
	physCollisionDebugSprite = spriteRenderer.AllocateSprite( physCollisionMap );
	physCollisionDebugSprite->x = 0.0f;
	physCollisionDebugSprite->y = 0.0f;
	physCollisionDebugSprite->SetSort( 200.0f );
	 */
}

bool physIsGround(int _x, int _y)
{
	if( !physTakeSample( _x, _y)) return false;
	return physSample[ 0 ] > 0.5f;	// Red channel is ground
}

bool physIsWall(int _x, int _y)
{
	if( !physTakeSample( _x, _y)) return false;
	return physSample[ 1 ] > 0.5f;	// Green channel is wall
}

bool physTakeSample( int _x, int _y )
{
	return pTileRenderer->Sample( _x, _y, physSample );
}
