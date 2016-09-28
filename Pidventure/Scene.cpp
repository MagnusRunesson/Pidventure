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
#include "Engine/IO/File.h"
#include "Engine/Core/Debug.h"
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
};

class CScene
{
public:
	CScene()
	{
		NumObjects = 0;
		aObjects = NULL;
	}
	
	uint32 NumObjects;
	CSceneObject* aObjects;
};

const int SCENE_MAX_SPRITES								= 2048;

GameObject* sceneObjects[ SCENE_MAX_SPRITES ];
Image* sceneImage[ SCENE_MAX_SPRITES ];

void sceneLoad( const char* _pszName )
{
	CSceneObject* pSceneObjects;
	int readBytes;

	char objectFileName[ 1024 ];
	sprintf( objectFileName, "%s.peso", _pszName );
	debugLog("Loading scene '%s'\n", _pszName );
	if( !fileLoad( objectFileName, (void**)&pSceneObjects, &readBytes ))
	{
		debugLog("Failed to load scene '%s'\n", objectFileName );
		return;
	}
	
	CScene* pScene = new CScene();
	pScene->NumObjects = readBytes / sizeof( CSceneObject );
	pScene->aObjects = pSceneObjects;

	debugLog( "num objects: %i\n", pScene->NumObjects );
	int i;
	
	for( i=0; i<pScene->NumObjects; i++ )
	{
		CSceneObject* pObj = &pScene->aObjects[ i ];
		debugLog( "Object %i: %s at %i,%i sort %.2f\n", i, pObj->pszDefinitionName, pObj->x, pObj->y, pObj->sort );
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
			sceneImage[ i ] = pImage;
		}
		
		sceneObjects[ i ] = pGO;
		
		Sprite* pSprite = pGO->GetSprite();
		pGO->SetWorldPosition( pObj->x, pObj->y );
		pSprite->SetSort( pObj->sort );
		
	}
	
	physInit();
}
