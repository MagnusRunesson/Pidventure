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
	
	const char* pszMainScene;
	const char* apszScene[10];
	int numDoors;
	CWorldDoorData aDoors[10];	// Door x always lead to apszScene[x]
};

static CDoor* g_pDoor[10];
static CScene* g_pMainScene;
static CScene* g_apSubScene[10];
static int g_numDoors;

extern CPlayer* pPlayer;

void worldInit()
{
	g_pMainScene = new CScene();
	g_pMainScene->Load( "scene_highlands_home" );
	g_pMainScene->SetSort( -1.2f );
	
	cameraSetBounds( g_pMainScene );
	
	g_apSubScene[ 0 ] = new CScene();
	g_apSubScene[ 0 ]->Load( "scene_highlands_interior_test" );
	g_apSubScene[ 0 ]->SetWorldPosition( 8*4, 8*4 );
	g_apSubScene[ 0 ]->SetSort( -1.1f );
	
	g_apSubScene[ 1 ] = new CScene();
	g_apSubScene[ 1 ]->Load( "scene_highlands_home_interior_townhouse" );
	g_apSubScene[ 1 ]->SetWorldPosition( 40*4, 1*4 );
	g_apSubScene[ 1 ]->SetSort( -1.1f );
	
	debugLog("Gamesetup start 6\n");
	
	debugLog("DoorManager init\n");
	doorManager.Init();
	
	debugLog("DoorManager init done\n");
	
	g_pDoor[ 0 ] = doorManager.CreateDoor( 46, 52 );
	g_pDoor[ 0 ]->m_pSceneInside = g_apSubScene[ 0 ];
	g_pDoor[ 0 ]->m_pSceneOutside = g_pMainScene;
	
	g_pDoor[ 1 ] = doorManager.CreateDoor( (46*4)+2, 13*4 );
	g_pDoor[ 1 ]->m_pSceneInside = g_apSubScene[ 1 ];
	g_pDoor[ 1 ]->m_pSceneOutside = g_pMainScene;
	
	g_numDoors = 2;
	
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
