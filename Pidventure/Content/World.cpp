//
//  World.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2016-10-26.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include <stdlib.h>
#include "Engine/Core/Debug.h"
#include "Pidventure/Gameplay/Door.h"
#include "Pidventure/Content/World.h"
#include "Pidventure/CameraController.h"
#include "Pidventure/Physics.h"
#include "Pidventure/Scene.h"
#include "Pidventure/Player/Player.h"
#include "Pidventure/Player/PlayerAvatar.h"

class CWorldData
{
public:
	class CWorldDoorData
	{
	public:
		int worldX;
		int worldY;
	};
	
	class CSubScene
	{
	public:
		const char* pszName;
		int worldX;
		int worldY;
	};
	
	const char* pszMainScene;
	int numSubScenes;
	CSubScene aSubScene[10];
	CWorldDoorData aDoor[10];	// Door x always lead to apszScene[x]
};

static CDoor* g_pDoor[10];
static CScene* g_pMainScene;
static CScene* g_apSubScene[10];
static int g_numDoors;

extern CPlayer* pPlayer;

CWorldData firstWorld {
	"scene_highlands_home",
	2,
	{
		{ "scene_highlands_interior_test", 8*4, 8*4 },
		{ "scene_highlands_home_interior_townhouse", 40*4, 1*4 },
	},
	{
		{ (11*4)+2, 13*4 },
		{ (46*4)+2, 13*4 },
	}
};

void worldInit()
{
	debugLog("DoorManager init\n");
	doorManager.Init();
	debugLog("DoorManager init done\n");
	
	g_pMainScene = new CScene();
	g_pMainScene->Load( firstWorld.pszMainScene );
	g_pMainScene->SetSort( -1.2f );
	
	cameraSetBounds( g_pMainScene );
	
	int subScene;
	for( subScene=0; subScene<firstWorld.numSubScenes; subScene++ )
	{
		//
		// Create subscene
		//
		CWorldData::CSubScene* pSubSceneDefinition = &firstWorld.aSubScene[ subScene ];
		CScene* pSubScene = new CScene();
		pSubScene->Load( pSubSceneDefinition->pszName );
		pSubScene->SetWorldPosition( pSubSceneDefinition->worldX, pSubSceneDefinition->worldY );
		pSubScene->SetSort( -1.1f );
		g_apSubScene[ subScene ] = pSubScene;
		
		//
		// Create door that leads into subscene
		//
		CDoor* pDoor = doorManager.CreateDoor( firstWorld.aDoor[ subScene ].worldX, firstWorld.aDoor[ subScene ].worldY );
		pDoor->m_pSceneInside = pSubScene;
		pDoor->m_pSceneOutside = g_pMainScene;
		g_pDoor[ subScene ] = pDoor;
	}
	
	debugLog("Gamesetup start 6\n");
	
	g_numDoors =firstWorld.numSubScenes;
	
	physInit( g_pMainScene );
}

void worldUpdate()
{
	if( pPlayer->m_pAvatar->m_worldX < g_pMainScene->GetWorldLeft() + 2.0f ) debugLog( "OUtside left!\n" );
	if( pPlayer->m_pAvatar->m_worldX > g_pMainScene->GetWorldRight() - 2.0f ) debugLog( "OUtside right!\n" );
	
	g_pMainScene->SetViewportTopLeft((int)cameraWorldX(), (int)cameraWorldY());
	g_pMainScene->Render();

	int i;
	for( i=0; i<g_numDoors; i++ )
	{
		if( g_pDoor[ i ]->IsOpen())
		{
			g_pDoor[ i ]->m_pSceneInside->SetViewportTopLeft((int)cameraWorldX(), (int)cameraWorldY());
			g_pDoor[ i ]->m_pSceneInside->Render();
		}
	}
}

void worldInteract()
{
	CDoor* pDoor = doorManager.GetDoorAt( pPlayer->m_pAvatar->m_worldX, pPlayer->m_pAvatar->m_worldY );
	if( pDoor != NULL )
	{
		if( !pDoor->IsOpen())
		{
			pDoor->Open();
			physSetActiveScene( pDoor->m_pSceneInside );
		} else {
			pDoor->Close();
			physSetActiveScene( pDoor->m_pSceneOutside );
		}
	}
}
