//
//  World.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2016-10-26.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include <stdlib.h>
#include "Engine/Core/Debug.h"
#include "Engine/Core/Memory.h"
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
static CScene g_MainScene;
static CScene g_aSubScene[10];
static int g_numDoors;

extern CPlayer* pPlayer;

CWorldData firstWorld {
	"scene_highlands_home",
	3,
	{
		{ "scene_highlands_interior_test", 8*4, 8*4 },
		{ "scene_highlands_home_interior_townhouse", 40*4, 1*4 },
		{ "scene_highlands_home_interior_treehouse", 79*4, 3*4 },
	},
	{
		{ (11*4)+2, 13*4 },
		{ (46*4)+2, 13*4 },
		{ ((79+7)*4)+2, (3+5)*4 },
	}
};

void worldLoad()
{
	g_MainScene.Load( firstWorld.pszMainScene );
	g_MainScene.SetSort( -1.2f );

	cameraSetBounds( &g_MainScene );
	
	int subScene;
	for( subScene=0; subScene<firstWorld.numSubScenes; subScene++ )
	{
		//
		// Create subscene
		//
		CWorldData::CSubScene* pSubSceneDefinition = &firstWorld.aSubScene[ subScene ];
		CScene* pSubScene = &g_aSubScene[ subScene ];
		pSubScene->Load( pSubSceneDefinition->pszName );
		pSubScene->SetWorldPosition( pSubSceneDefinition->worldX, pSubSceneDefinition->worldY );
		pSubScene->SetSort( -1.1f );
		
		//
		// Create door that leads into subscene
		//
		CDoor* pDoor = doorManager.CreateDoor( firstWorld.aDoor[ subScene ].worldX, firstWorld.aDoor[ subScene ].worldY );
		pDoor->m_pSceneInside = pSubScene;
		pDoor->m_pSceneOutside = &g_MainScene;
		g_pDoor[ subScene ] = pDoor;
	}

	g_numDoors = firstWorld.numSubScenes;
}

void worldUnload()
{
	doorManager.Reset();
	
	g_MainScene.Unload();
	
	int subScene;
	for( subScene=0; subScene<firstWorld.numSubScenes; subScene++ )
	{
		g_pDoor[ subScene ] = NULL;
		g_aSubScene[ subScene ].Unload();
	}
}

void worldInit()
{
	debugLog("DoorManager init\n");
	doorManager.Init();
	debugLog("DoorManager init done\n");

	worldLoad();
	
	
	debugLog("Gamesetup start 6\n");
	
	physInit( &g_MainScene );
}

void worldExit()
{
	worldUnload();
}

void worldUpdate()
{
	if( pPlayer->m_Avatar.m_worldX < g_MainScene.GetWorldLeft() + 2.0f ) debugLog( "OUtside left!\n" );
	if( pPlayer->m_Avatar.m_worldX > g_MainScene.GetWorldRight() - 2.0f ) debugLog( "OUtside right!\n" );
	
	g_MainScene.SetViewportTopLeft((int)cameraWorldX(), (int)cameraWorldY());
	g_MainScene.Render();

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
	CDoor* pDoor = doorManager.GetDoorAt( pPlayer->m_Avatar.m_worldX, pPlayer->m_Avatar.m_worldY );
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
