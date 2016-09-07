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
#include "Engine/IO/Joypad.h"
#include "Pidventure/Pidventure.h"

float t;

CPlayer* pPlayer;
Image* pTestImage;
Sprite* pPlayerSprite;

void game_setup()
{
	pPlayer = new CPlayer();

	pTestImage = imageLoad("herotest.pei");
	pPlayerSprite = spriteRenderer.AllocateSprite(pTestImage);
	
	pPlayerSprite->x = 40.0f;
	pPlayerSprite->y = 10.0f;
	
	t = 0.0f;
}

void game_loop()
{
	float dt = 1.0f / 60.0f;
	
	pPlayer->Update();
	
	t += dt * 0.5f;
	if( t > 2.0f )
		t -= 2.0f;
	
	float g = t;
	if(g > 1.0f )
		g = 2.0f-g;
	
	int x, y;
	for(y=0; y<SCREEN_HEIGHT; y++)
	{
		for(x=0; x<SCREEN_WIDTH; x++)
		{
			float fx = ((float)x) / ((float)SCREEN_WIDTH);
			float fy = ((float)y) / ((float)SCREEN_HEIGHT);
			int wrofs = (y*SCREEN_WIDTH)+x;
			screenBuffer[wrofs*4 + 0] = fx;
			screenBuffer[wrofs*4 + 1] = g;
			screenBuffer[wrofs*4 + 2] = fy;
		}
	}
	
	if(padGetKeys() & PAD_KEYMASK_PRIMARY)
	{
		screenBuffer[0] = 1.0f;
	}
	
	spriteRenderer.Render();
}

void game_debugTrigger(int _id)
{
	debugLog("DebugTrigger %i\n", _id);
}
