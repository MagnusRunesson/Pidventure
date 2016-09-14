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
#include "Engine/Scene/Camera.h"
#include "Engine/IO/Joypad.h"
#include "Pidventure/Pidventure.h"
#include "Pidventure/background.h"
#include "Pidventure/Scene.h"

float t;

CPlayer* pPlayer;
Camera camera;

void game_setup()
{
	Camera::main = &camera;

	pPlayer = new CPlayer();
	pPlayer->SetWorldPosition( 40.0f, 59.0f );

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

	const float CAMERA_BOUNDS = 30.0f;
	int camPlayerDiff = pPlayer->m_pAvatar->m_worldX - camera.GetWorldX();
	if( camPlayerDiff < CAMERA_BOUNDS )
		camera.SetWorldPosition(camera.GetWorldX()-1.0f, camera.GetWorldY());
	if( camPlayerDiff > SCREEN_WIDTH - CAMERA_BOUNDS )
		camera.SetWorldPosition(camera.GetWorldX()+1.0f, camera.GetWorldY());
	
	bgSetCameraPosition( camera.GetWorldX(), 0.0f );
	gameObjectManager.Render();
	spriteRenderer.Render();
}

void game_debugTrigger(int _id)
{
	debugLog("DebugTrigger %i\n", _id);
}
