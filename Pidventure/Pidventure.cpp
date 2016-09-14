//
//  Pidventure.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-01.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include "Engine/Core/Debug.h"
#include "Engine/Graphics/Screen.h"
#include "Engine/Graphics/Image.h"
#include "Engine/Graphics/SpriteRenderer.h"
#include "Engine/Scene/GameObjectManager.h"
#include "Engine/IO/Joypad.h"
#include "Pidventure/Pidventure.h"
#include "Pidventure/background.h"
#include "Pidventure/Scene.h"
#include "Pidventure/CameraController.h"

float t;

CPlayer* pPlayer;

void game_setup()
{

	pPlayer = new CPlayer();
	pPlayer->SetWorldPosition( 40.0f, 59.0f );

	cameraInit( pPlayer->m_pAvatar );
	bgInit();
	sceneLoad( "testfest" );
	
	t = 0.0f;
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
		}
	}
	
	if(padGetKeys() & PAD_KEYMASK_PRIMARY)
	{
		screenBuffer[0] = 1.0f;
	}

	cameraUpdate();
	bgSetCameraPosition( cameraWorldX(), 0.0f );
	gameObjectManager.Render();
	spriteRenderer.Render();
}

void game_debugTrigger(int _id)
{
	debugLog("DebugTrigger %i\n", _id);
}
