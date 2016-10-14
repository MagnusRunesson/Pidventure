//
//  Pidventure.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-01.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include <stdlib.h>
#include "Engine/Core/Debug.h"
#include "Engine/Graphics/Screen.h"
#include "Engine/Graphics/Image.h"
#include "Engine/Graphics/SpriteRenderer.h"
#include "Engine/Scene/GameObjectManager.h"
#include "Engine/IO/Joypad.h"
#include "Engine/Graphics/Animation.h"
#include "Engine/Graphics/TileBank.h"
#include "Engine/Graphics/TileMap.h"
#include "Engine/Graphics/TileRenderer.h"
#include "Pidventure/Pidventure.h"
#include "Pidventure/background.h"
#include "Pidventure/Scene.h"
#include "Pidventure/CameraController.h"
#include "Pidventure/Gameplay/Door.h"
#include "Pidventure/DataManual/Data.h"

float t;

CPlayer* pPlayer;
static CTileBank* pTileBank;
static CTileMap* pTileMap;
static TileRenderer* pTileRenderer;

CDoor* g_pDoor;

void game_setup()
{
	debugLog("Gamesetup start - banankontakt\n");	// This was printed
	char* papa = (char*)malloc(10);
	debugLog("Gamesetup start - kossa\n");			// And this was printed
	pPlayer = new CPlayer();
	debugLog("Gamesetup start - apa\n");			// But this was never printed. Added a bunch of prints in between, that should help find out what is going on.
	pPlayer->SetWorldPosition( 40.0f, 943.0f );

	debugLog("Gamesetup start 2\n");
	dataInit();
	debugLog("Gamesetup start 3\n");
	cameraInit( pPlayer->m_pAvatar );
	debugLog("Gamesetup start 4\n");
	bgInit();
	debugLog("Gamesetup start 5\n");
	sceneLoad( "scene_highlands" );
	debugLog("Gamesetup start 6\n");
	pTileBank = tilebankLoad( "tilebank_highlands" );
	debugLog("Gamesetup start 7\n");
	pTileMap = tilemapLoad( "scene_highlands" );
	debugLog("Gamesetup start 8\n");
	pTileRenderer = new TileRenderer( pTileMap, pTileBank );
	debugLog("Gamesetup start 9\n");
	pTileRenderer->SetDepth( -1.0f );

	debugLog("DoorManager init\n");
	doorManager.Init();

	debugLog("DoorManager init done\n");
	
	g_pDoor = doorManager.CreateDoor( 46, 936 );
	
	t = 0.0f;
	debugLog("Gamesetup end\n");
}

void game_loop()
{
	float dt = 1.0f / 60.0f;
	
	pPlayer->Update();

	t += dt * 0.5f;
	
	int x, y;
	for(y=0; y<SCREEN_HEIGHT; y++)
	{
		for(x=0; x<SCREEN_WIDTH; x++)
		{
			int wrofs = (y*SCREEN_WIDTH)+x;
			screenBuffer[wrofs*4 + 0] = 1.0f;
			screenBuffer[wrofs*4 + 1] = 1.0f;
			screenBuffer[wrofs*4 + 2] = 1.0f;
			screenBuffer[wrofs*4 + 3] = -1000.0f;
		}
	}
	
	gameObjectManager.Update();
	cameraUpdate();
	bgSetCameraPosition( cameraWorldX(), 0.0f );
	pTileRenderer->SetPosition( (int)cameraWorldX(), (int)cameraWorldY());
	pTileRenderer->Render();
	gameObjectManager.Render();
	spriteRenderer.Render();
}

void game_debugTrigger(int _id)
{
	/*
	debugLog("DebugTrigger %i\n", _id);
	if(_id == 1 )
		g_pDoor->Open();
	
	if( _id == 2 )
		g_pDoor->Close();
	 */
	
	if( _id == 1 )
	{
		CDoor* pDoor = doorManager.GetDoorAt( pPlayer->m_pAvatar->m_worldX, pPlayer->m_pAvatar->m_worldY );
		if( pDoor != NULL )
			pDoor->Open();
	}

	if( _id == 2 )
	{
		CDoor* pDoor = doorManager.GetDoorAt( pPlayer->m_pAvatar->m_worldX, pPlayer->m_pAvatar->m_worldY );
		if( pDoor != NULL )
			pDoor->Close();
	}
}
