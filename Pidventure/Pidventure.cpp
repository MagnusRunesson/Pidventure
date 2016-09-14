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
#include "Pidventure/Pidventure.h"
#include "Pidventure/background.h"
#include "Pidventure/Scene.h"
#include "Pidventure/CameraController.h"

float t;

AnimationFrameDefinition frames[] = {
	{
		"sprite_prop_flower_red_af00",
		NULL,
		4,
		0,
		1.0f, 4.0f,
	},
	{
		"sprite_prop_flower_red_af01",
		NULL,
		4,
		0,
		2.0f, 4.0f,
	},
	{
		"sprite_prop_flower_red_af02",
		NULL,
		4,
		0,
		2.0f, 3.0f,
	},
	{
		"sprite_prop_flower_red_af03",
		NULL,
		4,
		0,
		1.0f, 3.0f,
	},
};

AnimationSequenceDefinition anim = {
	sizeof(frames) / sizeof(AnimationFrameDefinition), 0, true, frames,
};

CPlayer* pPlayer;
GameObject* pAnimatedFlowers;

void game_setup()
{

	pPlayer = new CPlayer();
	pPlayer->SetWorldPosition( 40.0f, 59.0f );

	cameraInit( pPlayer->m_pAvatar );
	bgInit();
	sceneLoad( "testfest" );

	anim.LoadImages();

	pAnimatedFlowers = gameObjectManager.CreateGameObject( &anim );
	pAnimatedFlowers->SetWorldPosition(30.0f, 30.0f);
	pAnimatedFlowers->GetAnimation()->Play();
	
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

	gameObjectManager.Update();
	cameraUpdate();
	bgSetCameraPosition( cameraWorldX(), 0.0f );
	gameObjectManager.Render();
	spriteRenderer.Render();
}

void game_debugTrigger(int _id)
{
	debugLog("DebugTrigger %i\n", _id);
}
