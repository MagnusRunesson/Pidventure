//
//  Physics.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-13.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include "Engine/stdc/stdc.h"
#include "Engine/Core/Memory.h"
#include "Engine/Graphics/Image.h"
#include "Engine/Graphics/SpriteRenderer.h"
#include "Engine/Graphics/TileBank.h"
#include "Engine/Graphics/TileMap.h"
#include "Engine/Graphics/TileRenderer.h"
#include "Engine/Scene/Scene.h"
#include "Pidventure/Physics.h"

Image* physCollisionMap;
Sprite* physCollisionDebugSprite;
//static CTileBank* pTileBank;
//static CTileMap* pTileMap;
static CTileRenderer physTileRenderer( NULL, NULL );
float physSample[ 4 ];

void physInit( CScene* _pScene )
{
	//physCollisionMap = imageLoad( "sprite_collisiontest" );
	/*pTileBank = tilebankLoad( "tilebank_highlands_collision" );
	pTileMap = tilemapLoad( "scene_highlands" );
	pTileRenderer = new TileRenderer( pTileMap, pTileBank );*/
	/*
	physCollisionDebugSprite = spriteRenderer.AllocateSprite( physCollisionMap );
	physCollisionDebugSprite->x = 0.0f;
	physCollisionDebugSprite->y = 0.0f;
	physCollisionDebugSprite->SetSort( 200.0f );
	 */
	//pTileRenderer = new TileRenderer( NULL, NULL );
	physSetActiveScene( _pScene );
}

bool physIsGround(int _x, int _y)
{
	if( !physTakeSample( _x, _y)) return true;
	return physSample[ 0 ] > 0.5f;	// Red channel is ground
}

bool physIsWall(int _x, int _y)
{
	if( !physTakeSample( _x, _y)) return false;
	return physSample[ 1 ] > 0.5f;	// Green channel is wall
}

bool physTakeSample( int _x, int _y )
{
	return physTileRenderer.Sample( _x, _y, physSample );
}

void physSetActiveScene( CScene* _pScene )
{
	physTileRenderer.m_pTileBank = _pScene->pTileBankCollision;
	physTileRenderer.m_pTileMap = _pScene->pTileMap;
	physTileRenderer.m_x = _pScene->m_worldX;
	physTileRenderer.m_y = _pScene->m_worldY;
}
