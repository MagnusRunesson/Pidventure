//
//  Scene.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-12.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include "Engine/stdc/stdc.h"
#include "Engine/Util/String.h"
#include "Engine/Scene/GameObjectManager.h"
#include "Engine/Graphics/Image.h"
#include "Engine/Graphics/Sprite.h"
#include "Engine/Graphics/Animation.h"
#include "Engine/Graphics/TileMap.h"
#include "Engine/Graphics/TileBank.h"
#include "Engine/Graphics/TileRenderer.h"
#include "Engine/IO/File.h"
#include "Engine/IO/Data.h"
#include "Engine/Core/Debug.h"
#include "Engine/Core/Memory.h"
#include "Engine/Scene/Scene.h"

#define SO_FLAG_ANIMATED		(1<<0)
#define SO_FLAG_FLIP_X			(1<<1)
#define SO_FLAG_FLIP_Y			(1<<2)

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
	Reset();
}

CScene::CScene( const char* _pszSceneName, const char* _pszTileBankName, const char* _pszCollisionTileBankName ) : TileRenderer( NULL, NULL )
{
	Reset();
	Load( _pszSceneName, _pszTileBankName, _pszCollisionTileBankName );
}

CScene::~CScene()
{
	Unload();
}

void CScene::Reset()
{
	SetViewportTopLeft( 0, 0 );
	SetWorldPosition( 0, 0 );
	SetSort( 0.0f );
	pTileBank = NULL;
	pTileBankCollision = NULL;
	pTileMap = NULL;
}

bool CScene::Load( const char* _pszSceneName, const char* _pszTileBankName, const char* _pszCollisionTileBankName )
{
	CSceneObject* pSceneObjects;
	int readBytes;

	char objectFileName[ 1024 ];
	stringCombine( objectFileName, 1024, _pszSceneName, ".peso" );
	
	debugLog("Loading scene '%s'\n", _pszSceneName );
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
	unsigned int i;
	
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
		if( pObj->flags & SO_FLAG_ANIMATED )
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

		pGO->GetSprite()->SetFlippedX( pObj->flags & SO_FLAG_FLIP_X );
		pGO->GetSprite()->SetFlippedY( pObj->flags & SO_FLAG_FLIP_Y );
	}
	
	pTileBank = tilebankLoad( _pszTileBankName );
	debugLog("Gamesetup start 7a\n");
	pTileBankCollision = NULL;
	if( _pszCollisionTileBankName != NULL )
	{
		pTileBankCollision = tilebankLoad( _pszCollisionTileBankName );
		debugLog("Gamesetup start 7b\n");
	}
	pTileMap = tilemapLoad( _pszSceneName );
	debugLog("Gamesetup start 8\n");
	TileRenderer.m_pTileBank = pTileBank;
	TileRenderer.m_pTileMap = pTileMap;
	debugLog("Gamesetup start 9\n");
	TileRenderer.SetDepth( -1.0f );

	fileUnload( pSceneObjects );

	return true;
}

void CScene::Unload()
{
	if( pTileBank == NULL )
	{
		// The scene have already been unloaded
		return;
	}
	
	tilebankUnload( pTileBank );
	pTileBank = NULL;
	if( pTileBankCollision != NULL )
	{
		tilebankUnload( pTileBankCollision );
		pTileBankCollision = NULL;
	}
	
	tilemapUnload( pTileMap );
	pTileMap = NULL;

	int i;
	for( i=0; i<MAX_GAMEOBJECTS; i++ )
	{
		GameObject* pGO = sceneObjects[ i ];
		if(!pGO)
			continue;
		
		//
		if(!pGO->GetAnimationSequenceDefinition())
		{
			// Not an animated object. That means it has an image that we should free up.
			Sprite* pSprite = pGO->GetSprite();
			imageUnload( pSprite->image );
		}
		
		gameObjectManager.FreeGameObject( pGO );
	}
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
