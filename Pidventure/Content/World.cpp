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

static CDoor* g_pDoor[10];
static CScene* g_pScene;
static CScene* g_pScene2;
static CScene* g_pScene3;

void worldInit()
{
	g_pScene = new CScene();
	g_pScene->Load( "scene_highlands_home" );
	g_pScene->SetSort( -1.2f );
	
	cameraSetBounds( g_pScene );
	
	g_pScene2 = new CScene();
	g_pScene2->Load( "scene_highlands_interior_test" );
	g_pScene2->SetWorldPosition( 8*4, 8*4 );
	g_pScene2->SetSort( -1.1f );
	
	g_pScene3 = new CScene();
	g_pScene3->Load( "scene_highlands_home_interior_townhouse" );
	g_pScene3->SetWorldPosition( 40*4, 1*4 );
	g_pScene3->SetSort( -1.1f );
	
	debugLog("Gamesetup start 6\n");
	
	debugLog("DoorManager init\n");
	doorManager.Init();
	
	debugLog("DoorManager init done\n");
	
	g_pDoor[0] = doorManager.CreateDoor( 46, 52 );
	g_pDoor[0]->m_pSceneInside = g_pScene2;
	g_pDoor[0]->m_pSceneOutside = g_pScene;
	
	g_pDoor[1] = doorManager.CreateDoor( (46*4)+2, 13*4 );
	g_pDoor[1]->m_pSceneInside = g_pScene3;
	g_pDoor[1]->m_pSceneOutside = g_pScene;
	
	physInit( g_pScene );
}

void worldUpdate()
{
	g_pScene->SetViewportTopLeft((int)cameraWorldX(), (int)cameraWorldY());
	g_pScene->Render();

	if(g_pDoor[0]->IsOpen())
	{
		g_pDoor[0]->m_pSceneInside->SetViewportTopLeft((int)cameraWorldX(), (int)cameraWorldY());
		g_pDoor[0]->m_pSceneInside->Render();
	}
	
	if(g_pDoor[1]->IsOpen())
	{
		g_pDoor[1]->m_pSceneInside->SetViewportTopLeft((int)cameraWorldX(), (int)cameraWorldY());
		g_pDoor[1]->m_pSceneInside->Render();
	}
}

void worldInteract(CPlayer* _pPlayer)
{
	CDoor* pDoor = doorManager.GetDoorAt( _pPlayer->m_pAvatar->m_worldX, _pPlayer->m_pAvatar->m_worldY );
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
