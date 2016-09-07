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
#include "Engine/IO/Joypad.h"
#include "Pidventure/Pidventure.h"

float t;

CPlayer* pPlayer;
Image* pTestImage;

void game_setup()
{
	pPlayer = new CPlayer();

	pTestImage = imageLoad("herotest.pei");
	debugLog("pTestImage=0x%016llx\n", pTestImage);
	debugLog("    pixels=0x%016llx\n", pTestImage->pixels);
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
	
	int basex = 10;
	int basey = 10;
	for(y=0; y<pTestImage->h; y++)
	{
		for(x=0; x<pTestImage->w; x++)
		{
			int readofs = ((y*pTestImage->w) + x) * 4;
			int writeOfs = (((basey+y)*SCREEN_WIDTH) + (basex+x)) * 4;
			float r = pTestImage->pixels[ readofs+0 ];
			float g = pTestImage->pixels[ readofs+1 ];
			float b = pTestImage->pixels[ readofs+2 ];
			float a = pTestImage->pixels[ readofs+3 ];
			float ia = 1.0f-a;
			screenBuffer[ writeOfs+0 ] = (r*a)+(screenBuffer[ writeOfs+0 ]*ia);
			screenBuffer[ writeOfs+1 ] = (g*a)+(screenBuffer[ writeOfs+1 ]*ia);
			screenBuffer[ writeOfs+2 ] = (b*a)+(screenBuffer[ writeOfs+2 ]*ia);
			//screenBuffer[ writeOfs+3 ] = (a;
		}
	}
	
	if(padGetKeys() & PAD_KEYMASK_PRIMARY)
	{
		screenBuffer[0] = 1.0f;
	}
}

void game_debugTrigger(int _id)
{
	debugLog("DebugTrigger %i\n", _id);
}
