//
//  Joypad.cpp
//  TinyArcade
//
//  Created by Magnus Runesson on 2015-12-03.
//  Copyright © 2015 Magnus Runesson. All rights reserved.
//

#ifdef ENGINE_TARGET_RPI
#include <wiringPi.h>
#endif

#include "Engine/Core/Types.h"
#include "Engine/IO/Joypad.h"
#include "Engine/Core/BitHelpers.h"
#include "Engine/Core/Debug.h"

#ifndef ENGINE_TARGET_TINYARCADE

extern int gKeyDPadBuff;

#ifdef ENGINE_TARGET_RPI
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
#endif

#define BUFFER_SWITCH_LENGTH	(16)
#define BUFFER_SWITCH_MASK		((1<<BUFFER_SWITCH_LENGTH)-1)

uint32 gkeys;
uint32 gKeyBuff;

void padInit()
{
#ifdef ENGINE_TARGET_RPI
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
#endif
}

void padUpdate()
{
#ifdef ENGINE_TARGET_RPI
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
#endif
	gkeys <<= BUFFER_SWITCH_LENGTH;
	gkeys |= gKeyBuff;
}

uint8 padGet( float* _x, float* _y )
{
	*_x = padGetX();
	*_y = padGetY();

	return gkeys;
}

uint8 padGetKeys()
{
	return gkeys;
}

float padGetX()
{
	float ret = 0;
	if( HasBit( gkeys, PAD_KEYMASK_DPAD_LEFT ))	ret -= 1.0f;
	if( HasBit( gkeys, PAD_KEYMASK_DPAD_RIGHT ))	ret += 1.0f;
	
	return ret;
}

float padGetY()
{
	float ret = 0;
	if( HasBit( gkeys, PAD_KEYMASK_DPAD_UP ))	ret -= 1.0f;
	if( HasBit( gkeys, PAD_KEYMASK_DPAD_DOWN ))	ret += 1.0f;
	
	return ret;
}

uint16 padGetPressed()
{
	uint16 prev = (gkeys>>BUFFER_SWITCH_LENGTH) & BUFFER_SWITCH_MASK;
	uint16 curr = (gkeys) & BUFFER_SWITCH_MASK;
	uint16 ret = (prev ^ curr) & curr;
	return ret;
}

uint16 padGetReleased()
{
	uint16 prev = (gkeys>>BUFFER_SWITCH_LENGTH) & BUFFER_SWITCH_MASK;
	uint16 curr = (gkeys) & BUFFER_SWITCH_MASK;
	uint16 ret = (prev ^ curr) & prev;
	return ret;
}

#else

uint8 gkeys;
int gKeyBuff;
int padX, padY;

#include <SPI.h>
#include <Wire.h>

#define TinyArcadePinX 42
#define TinyArcadePinY 1
#define TinyArcadePin1 45
#define TinyArcadePin2 44

void padInit()
{
	pinMode( TinyArcadePin1, INPUT_PULLUP );
	pinMode( TinyArcadePin2, INPUT_PULLUP );
	
	gKeyBuff = 0;
	gkeys = 0;
	padX = 0;
	padY = 0;
}

void padUpdate()
{
	const int deadzone = 125;
	padX = 0;
	padY = 0;
	int rx = analogRead( TinyArcadePinX );
	int ry = analogRead( TinyArcadePinY );
	if( rx < (512-deadzone)) padX = 1;
	if( rx > (512+deadzone)) padX = -1;
	if( ry < (512-deadzone)) padY = -1;
	if( ry > (512+deadzone)) padY = 1;
	
	gkeys <<= 4;
	if( !digitalRead( TinyArcadePin2 ))
		gkeys |= PAD_KEYMASK_PRIMARY;

	if( !digitalRead( TinyArcadePin1 ))
		gkeys |= PAD_KEYMASK_SECONDARY;
}

uint8 padGet( sint8* _x, sint8* _y )
{
	*_x = padX;
	*_y = padY;
	
	return gkeys;
}

uint8 padGetKeys()
{
	return gkeys;
}

sint8 padGetX()
{
	return padX;
}

sint8 padGetY()
{
	return padY;
}

uint8 padGetPressed()
{
	uint8 prev = (gkeys>>4) & 0x0f;
	uint8 curr = (gkeys) & 0x0f;
	uint8 ret = (prev ^ curr) & curr;
	return ret;
}

uint8 padGetReleased()
{
	uint8 prev = (gkeys>>4) & 0x0f;
	uint8 curr = (gkeys) & 0x0f;
	uint8 ret = (prev ^ curr) & prev;
	return ret;
}

uint16* screenBuffer;

uint32 nanos()
{
	return 0;
}

#endif
