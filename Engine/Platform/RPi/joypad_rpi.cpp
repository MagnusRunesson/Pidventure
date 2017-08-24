//
//  joypad_rpi.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2017-08-24.
//  Copyright © 2017 Magnus Runesson. All rights reserved.
//

#include <wiringPi.h>
#include "Engine/Core/Types.h"
#include "Engine/IO/Joypad.h"
#include "Engine/Core/BitHelpers.h"

const int RPI_GPIO_UP		= 19;
const int RPI_GPIO_DOWN		= 26;
const int RPI_GPIO_LEFT		= 13;
const int RPI_GPIO_RIGHT	= 6;
const int RPI_GPIO_BTN0		= 20;
const int RPI_GPIO_BTN1		= 16;
const int RPI_GPIO_BTN2		= 12;
const int RPI_GPIO_BTN3		= 22;
const int RPI_GPIO_BTN4		= 23;
const int RPI_GPIO_BTN5		= 24;
const int RPI_GPIO_BTN6		= 27;
const int RPI_GPIO_BTN7		= 21;

extern uint32 gKeyBuff;

void padHardwareInit()
{
	wiringPiSetupGpio();
	pullUpDnControl( RPI_GPIO_UP, PUD_UP );
	pullUpDnControl( RPI_GPIO_DOWN, PUD_UP );
	pullUpDnControl( RPI_GPIO_LEFT, PUD_UP );
	pullUpDnControl( RPI_GPIO_RIGHT, PUD_UP );
	pullUpDnControl( RPI_GPIO_BTN0, PUD_UP );
	pullUpDnControl( RPI_GPIO_BTN1, PUD_UP );
	pullUpDnControl( RPI_GPIO_BTN2, PUD_UP );
	pullUpDnControl( RPI_GPIO_BTN3, PUD_UP );
	pullUpDnControl( RPI_GPIO_BTN4, PUD_UP );
	pullUpDnControl( RPI_GPIO_BTN5, PUD_UP );
	pullUpDnControl( RPI_GPIO_BTN6, PUD_UP );
	pullUpDnControl( RPI_GPIO_BTN7, PUD_UP );
}

void padHardwareUpdate()
{
	gKeyBuff = 0;

	if( !digitalRead( RPI_GPIO_UP ))
	{
		gKeyBuff |= PAD_KEYMASK_DPAD_UP;
	} else {
		gKeyBuff &= ~PAD_KEYMASK_DPAD_UP;
	}
	
	if( !digitalRead( RPI_GPIO_DOWN ))
	{
		gKeyBuff |= PAD_KEYMASK_DPAD_DOWN;
	} else {
		gKeyBuff &= ~PAD_KEYMASK_DPAD_DOWN;
	}

	if( !digitalRead( RPI_GPIO_LEFT ))
	{
		gKeyBuff |= PAD_KEYMASK_DPAD_LEFT;
	} else {
		gKeyBuff &= ~PAD_KEYMASK_DPAD_LEFT;
	}

	if( !digitalRead( RPI_GPIO_RIGHT ))
	{
		gKeyBuff |= PAD_KEYMASK_DPAD_RIGHT;
	} else {
		gKeyBuff &= ~PAD_KEYMASK_DPAD_RIGHT;
	}

	if( !digitalRead( RPI_GPIO_BTN0 )) gKeyBuff |= PAD_KEYMASK_ACTION_UL;
	if( !digitalRead( RPI_GPIO_BTN1 )) gKeyBuff |= PAD_KEYMASK_ACTION_UM;
	if( !digitalRead( RPI_GPIO_BTN2 )) gKeyBuff |= PAD_KEYMASK_ACTION_UR;
	if( !digitalRead( RPI_GPIO_BTN3 )) gKeyBuff |= PAD_KEYMASK_ACTION_LL;
	if( !digitalRead( RPI_GPIO_BTN4 )) gKeyBuff |= PAD_KEYMASK_ACTION_LM;
	if( !digitalRead( RPI_GPIO_BTN5 )) gKeyBuff |= PAD_KEYMASK_ACTION_LR;
	if( !digitalRead( RPI_GPIO_BTN6 )) gKeyBuff |= PAD_KEYMASK_PGM_L;
	if( !digitalRead( RPI_GPIO_BTN7 )) gKeyBuff |= PAD_KEYMASK_PGM_R;
}
