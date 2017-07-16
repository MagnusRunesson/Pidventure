//
//  Pidventure.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-01.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include "Engine/Core/Debug.h"
#include "Engine/Core/Memory.h"
#include "Engine/Graphics/Screen.h"
#include "Engine/Graphics/Image.h"
#include "Engine/Graphics/SpriteRenderer.h"
#include "Engine/Scene/GameObjectManager.h"
#include "Engine/IO/Joypad.h"
#include "Engine/IO/FileCache.h"
#include "Engine/IO/Data.h"
#include "Engine/Graphics/Animation.h"
#include "Engine/Graphics/TileBank.h"
#include "Engine/Graphics/TileMap.h"
#include "Engine/Graphics/TileRenderer.h"
#include "Engine/Scene/Scene.h"
#include "Pidventure/Pidventure.h"
#include "Pidventure/background.h"
#include "Pidventure/CameraController.h"
#include "Pidventure/Gameplay/Door.h"
#include "Pidventure/Content/World.h"
#include "Pidventure/Physics.h"

float t;

CPlayer* pPlayer;

Image* pImageOverlay;

void game_setup()
{
	debugLog("Gamesetup start - banankontakt\n");	// This was printed
	char* papa = (char*)malloc(10);
	debugLog("Gamesetup start - kossa\n");			// And this was printed
	pPlayer = new CPlayer();
	debugLog("Gamesetup start - apa\n");			// But this was never printed. Added a bunch of prints in between, that should help find out what is going on.
	pPlayer->SetWorldPosition( 40.0f, 62.0f );

	debugLog("Gamesetup start 2\n");
	dataInit();
	animationDataInit();
	debugLog("Gamesetup start 3\n");
	cameraInit( &pPlayer->m_Avatar );
	debugLog("Gamesetup start 4\n");
	bgInit();
	debugLog("Gamesetup start 5\n");
	
	worldInit();

	t = 0.0f;
	debugLog("Gamesetup end\n");

	pImageOverlay = imageLoad( "sprite_overlaytest" );
	
	free(papa);
}

void game_exit()
{
	delete pPlayer;
	
	imageUnload( pImageOverlay );
	
	worldExit();
	bgExit();
	dataExit();
	
	fileCacheReset();
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
	worldUpdate();

	if(padGetPressed() & PAD_KEYMASK_SELECT )
	{
		worldInteract();
	}

	gameObjectManager.Render();
	spriteRenderer.Render();
	
	/*
	for(y=0; y<SCREEN_HEIGHT; y++)
	{
		for(x=0; x<SCREEN_WIDTH; x++)
		{
			int ofs = (y*SCREEN_WIDTH)+x;
			float srcr = screenBuffer[ofs*4 + 0];
			float srcg = screenBuffer[ofs*4 + 1];
			float srcb = screenBuffer[ofs*4 + 2];
			float dstr = pImageOverlay->pixels[ofs*4 + 0];
			float dstg = pImageOverlay->pixels[ofs*4 + 1];
			float dstb = pImageOverlay->pixels[ofs*4 + 2];
			screenBuffer[ofs*4 + 0] = srcr*dstr;
			screenBuffer[ofs*4 + 1] = srcg*dstg;
			screenBuffer[ofs*4 + 2] = srcb*dstb;
		}
	}
	*/
	
	
	/* DEBUG OVERLAY for rendering the ground collision on screen
	for( y=0; y<SCREEN_HEIGHT; y++ )
	{
		for( x=0; x<SCREEN_WIDTH; x++ )
		{
			int samplex = cameraWorldX()+x;
			int sampley = cameraWorldY()+y;
			if(physIsGround( samplex, sampley ))
			{
				int wr = ((y*SCREEN_WIDTH) + x) * 4;
				screenBuffer[ wr+0 ] = 1.0f;
				screenBuffer[ wr+1 ] = 0.0f;
				screenBuffer[ wr+2 ] = 0.0f;
			}
		}
	}
	 */
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
	
	/*
	if( _id == 1 )
	{
		CDoor* pDoor = doorManager.GetDoorAt( pPlayer->m_pAvatar->m_worldX, pPlayer->m_pAvatar->m_worldY );
		if( pDoor != NULL )
			if( !pDoor->IsOpen())
			{
				pDoor->Open();
				physSetActiveScene( pDoor->m_pSceneInside );
			}
	}

	if( _id == 2 )
	{
		CDoor* pDoor = doorManager.GetDoorAt( pPlayer->m_pAvatar->m_worldX, pPlayer->m_pAvatar->m_worldY );
		if( pDoor != NULL )
			if( pDoor->IsOpen())
			{
				pDoor->Close();
				physSetActiveScene( pDoor->m_pSceneOutside );
			}
	}
	 */
}
