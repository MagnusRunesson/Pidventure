//
//  World.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2016-10-26.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include "Engine/stdc/stdlib.h"
#include "Engine/Core/Debug.h"
#include "Engine/Core/Memory.h"
#include "Engine/Scene/Scene.h"
#include "Pidventure/Gameplay/Door.h"
#include "Pidventure/Content/World.h"
#include "Pidventure/CameraController.h"
#include "Pidventure/Physics.h"
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
	
	class CExitPoint
	{
	public:
		int nextRoom;
		int nextRoomStartPoint;
	};
	
	class CStartPoints
	{
	public:
		int worldX;
		int worldY;
	};
	
	const char* pszMainScene;

	CExitPoint exitLeft;
	CExitPoint exitRight;
	
	int numSubScenes;
	CSubScene aSubScene[10];
	CWorldDoorData aDoor[10];		// Door x always lead to aSubScene[x]
	
	CStartPoints aStartingPoint[4];
};

static CDoor* g_pDoor[10];
static CScene g_MainScene;
static CScene g_aSubScene[10];
static int g_numDoors;

extern CPlayer* pPlayer;

CWorldData firstWorld {
	"scene_highlands_home",
	
	{
		// Exit left
		1, 1
	},
	{
		// Exit right
		2, 0
	},
	
	3,	// Num doors
	{
		{ "scene_highlands_interior_test", 8*4, 8*4 },
		{ "scene_highlands_home_interior_townhouse", 40*4, 1*4 },
		{ "scene_highlands_home_interior_treehouse", 79*4, 3*4 },
	},
	{
		{ (11*4)+2, 13*4 },
		{ (46*4)+2, 13*4 },
		{ ((79+7)*4)+2, (3+5)*4 },
	},
	{
		{ 20, 15*4 },		// When entering from the left
		{ 119*4, 15*4 },	// When entering from the right
		{ 40, 62 },			// Other spawn point
	}
};

CWorldData secondWorld {
	"scene_highlands_firstmonster",
	
	// Left / right world
	{
		-1, -1
	},
	{
		0, 0
	},
	
	// Num doors
	0,
	{ },
	{ },
	{
		{ 0, 0 },		// When entering from the left
		{ 118*4, 11*4 },
	}
};

CWorldData* apAllWorlds[] = {
	&firstWorld,
	&secondWorld,
};

CWorldData* g_pCurrentWorld;

void worldLoad(int _index);
void worldLoad(CWorldData* _pWorld);

void worldLoad()
{
	worldLoad( 0 );
}

void worldLoad(int _index)
{
	worldLoad( apAllWorlds[ _index ]);
}

void worldLoad(CWorldData* _pWorld)
{
	g_pCurrentWorld = _pWorld;
	
	g_MainScene.Load( g_pCurrentWorld->pszMainScene, "tilebank_highlands", "tilebank_highlands_collision" );
	g_MainScene.SetSort( -1.2f );

	cameraSetBounds( &g_MainScene );
	
	int subScene;
	for( subScene=0; subScene<g_pCurrentWorld->numSubScenes; subScene++ )
	{
		//
		// Create subscene
		//
		CWorldData::CSubScene* pSubSceneDefinition = &g_pCurrentWorld->aSubScene[ subScene ];
		CScene* pSubScene = &g_aSubScene[ subScene ];
		pSubScene->Load( pSubSceneDefinition->pszName, "tilebank_highlands", "tilebank_highlands_collision" );
		pSubScene->SetWorldPosition( pSubSceneDefinition->worldX, pSubSceneDefinition->worldY );
		pSubScene->SetSort( -1.1f );
		
		//
		// Create door that leads into subscene
		//
		CDoor* pDoor = doorManager.CreateDoor( g_pCurrentWorld->aDoor[ subScene ].worldX, g_pCurrentWorld->aDoor[ subScene ].worldY );
		pDoor->m_pSceneInside = pSubScene;
		pDoor->m_pSceneOutside = &g_MainScene;
		g_pDoor[ subScene ] = pDoor;
	}

	g_numDoors = g_pCurrentWorld->numSubScenes;

	physInit( &g_MainScene );
}

void worldUnload()
{
	doorManager.Reset();
	
	g_MainScene.Unload();
	
	int subScene;
	for( subScene=0; subScene<g_pCurrentWorld->numSubScenes; subScene++ )
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
}

void worldExit()
{
	worldUnload();
}

void worldSwitchTo( CWorldData::CExitPoint* _pExitPointInfo )
{
	worldUnload();
	CWorldData* pNextWorld = apAllWorlds[ _pExitPointInfo->nextRoom ];
	worldLoad( pNextWorld );

	CWorldData::CStartPoints* pStartPoint = &pNextWorld->aStartingPoint[ _pExitPointInfo->nextRoomStartPoint ];
	pPlayer->m_Avatar.SetWorldPosition( pStartPoint->worldX, pStartPoint->worldY );
}

void worldUpdate()
{
	if( pPlayer->m_Avatar.m_worldX < g_MainScene.GetWorldLeft() + 2.0f )
	{
		debugLog( "OUtside left!\n" );
		worldSwitchTo( &g_pCurrentWorld->exitLeft );
	}
	
	if( pPlayer->m_Avatar.m_worldX > g_MainScene.GetWorldRight() - 2.0f )
	{
		debugLog( "OUtside right!\n" );
		worldSwitchTo( &g_pCurrentWorld->exitRight );
	}
	
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
