//
//  Scene.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-12.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include "Engine/Scene/GameObjectManager.h"
#include "Engine/Graphics/Image.h"
#include "Engine/Graphics/Sprite.h"
#include "Engine/Graphics/Animation.h"
#include "Engine/Graphics/TileMap.h"
#include "Engine/Graphics/TileBank.h"
#include "Engine/Graphics/TileRenderer.h"
#include "Engine/IO/File.h"
#include "Engine/Core/Debug.h"
#include "Engine/Core/Memory.h"
#include "Pidventure/Scene.h"
#include "Pidventure/Physics.h"
#include "Pidventure/DataManual/Data.h"

#define SO_FLAG_ANIMATED		(1<<0)

class CSceneObject
{
public:
	const char pszDefinitionName[ 64 ];
	sint32 x;
	sint32 y;
	float sort;
	uint32 flags;
	uint8 collisionIndex;
	uint8 pad0;
	uint8 pad1;
	uint8 pad2;
};

class CSceneDefinition
{
public:
	CSceneDefinition()
	{
		NumObjects = 0;
		aObjects = NULL;
	}
	
	uint32 NumObjects;
	CSceneObject* aObjects;
};


CScene::CScene() : TileRenderer( NULL, NULL )
{
	m_worldX = 0;
	m_worldY = 0;
}

bool CScene::Load( const char* _pszName )
{
	CSceneObject* pSceneObjects;
	int readBytes;

	char objectFileName[ 1024 ];
	sprintf( objectFileName, "%s.peso", _pszName );
	debugLog("Loading scene '%s'\n", _pszName );
	if( !fileLoad( objectFileName, (void**)&pSceneObjects, &readBytes ))
	{
		debugLog("Failed to load scene '%s'\n", objectFileName );
		return false;
	}
	
	CSceneDefinition sceneDefinition;
	CSceneDefinition* pScene = &sceneDefinition;
	pScene->NumObjects = readBytes / sizeof( CSceneObject );
	pScene->aObjects = pSceneObjects;

	debugLog( "num objects: %i\n", pScene->NumObjects );
	int i;
	
	for( i=0; i<pScene->NumObjects; i++ )
	{
		CSceneObject* pObj = &pScene->aObjects[ i ];
		debugLog( "Object %i: %s at %i,%i sort %.2f, collision index %i\n", i, pObj->pszDefinitionName, pObj->x, pObj->y, pObj->sort, pObj->collisionIndex );
	}
	
	for( i=0; i<SCENE_MAX_SPRITES; i++ )
		sceneObjects[ i ] = NULL;

	for( i=0; i<pScene->NumObjects; i++ )
	{
		CSceneObject* pObj = &pScene->aObjects[ i ];
		GameObject* pGO = NULL;
		if( pObj->flags && SO_FLAG_ANIMATED )
		{
			debugLog("Game object is animated, yay\n");
			AnimationSequenceDefinition* pAnimation = dataGetAnimationSequenceDefinition( pObj->pszDefinitionName );
			pGO = gameObjectManager.CreateGameObject( pAnimation );
			pGO->GetAnimation()->Play();
		} else {
			Image* pImage = imageLoad( pObj->pszDefinitionName );
			pGO = gameObjectManager.CreateGameObject( pImage );
		}
		
		sceneObjects[ i ] = pGO;
		
		Sprite* pSprite = pGO->GetSprite();
		pGO->SetWorldPosition( pObj->x, pObj->y );
		pSprite->SetSort( pObj->sort );
		pSprite->collisionIndex = pObj->collisionIndex;
		
	}
	
	pTileBank = tilebankLoad( "tilebank_highlands" );
	debugLog("Gamesetup start 7a\n");
	pTileBankCollision = tilebankLoad( "tilebank_highlands_collision" );
	debugLog("Gamesetup start 7b\n");
	pTileMap = tilemapLoad( _pszName );
	debugLog("Gamesetup start 8\n");
	TileRenderer.m_pTileBank = pTileBank;
	TileRenderer.m_pTileMap = pTileMap;
	debugLog("Gamesetup start 9\n");
	TileRenderer.SetDepth( -1.0f );

	fileUnload( pSceneObjects );

	return true;
}

void CScene::SetWorldPosition( int _x, int _y )
{
	m_worldX = _x;
	m_worldY = _y;
}

void CScene::SetSort( float _sort )
{
	TileRenderer.SetDepth( _sort );
}

void CScene::SetViewportTopLeft( int _x, int _y )
{
	TileRenderer.SetPosition( _x-m_worldX, _y-m_worldY );
}

void CScene::Render()
{
	TileRenderer.Render();
}

int CScene::GetWorldTop()
{
	return m_worldY;
}

int CScene::GetWorldBottom()
{
	return m_worldY + pTileMap->Height*pTileBank->TileHeight;
}

int CScene::GetWorldLeft()
{
	return m_worldX;
}

int CScene::GetWorldRight()
{
	return m_worldX + pTileMap->Width*pTileBank->TileWidth;
}
