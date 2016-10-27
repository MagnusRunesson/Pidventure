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
const int RPI_GPIO_UP		= 26;
const int RPI_GPIO_DOWN		= 19;
const int RPI_GPIO_LEFT		= 13;
const int RPI_GPIO_RIGHT	= 6;
const int RPI_GPIO_START	= 12;
const int RPI_GPIO_BTN0		= 16;
const int RPI_GPIO_BTN1		= 20;
const int RPI_GPIO_BTN2		= 21;
#endif

uint8 gkeys;
int gKeyBuff;

void padInit()
{
#ifdef ENGINE_TARGET_RPI
	wiringPiSetupGpio();
	pullUpDnControl( RPI_GPIO_UP, PUD_UP );
	pullUpDnControl( RPI_GPIO_DOWN, PUD_UP );
	pullUpDnControl( RPI_GPIO_LEFT, PUD_UP );
	pullUpDnControl( RPI_GPIO_RIGHT, PUD_UP );
	pullUpDnControl( RPI_GPIO_START, PUD_UP );
	pullUpDnControl( RPI_GPIO_BTN0, PUD_UP );
	pullUpDnControl( RPI_GPIO_BTN1, PUD_UP );
	pullUpDnControl( RPI_GPIO_BTN2, PUD_UP );
#endif
}

void padUpdate()
{
#ifdef ENGINE_TARGET_RPI
	if( !digitalRead( RPI_GPIO_UP ))
	{
		gKeyDPadBuff |= PAD_KEYMASK_DPAD_UP;
	} else {
		gKeyDPadBuff &= ~PAD_KEYMASK_DPAD_UP;
	}
	
	if( !digitalRead( RPI_GPIO_DOWN ))
	{
		gKeyDPadBuff |= PAD_KEYMASK_DPAD_DOWN;
	} else {
		gKeyDPadBuff &= ~PAD_KEYMASK_DPAD_DOWN;
	}

	if( !digitalRead( RPI_GPIO_LEFT ))
	{
		gKeyDPadBuff |= PAD_KEYMASK_DPAD_LEFT;
	} else {
		gKeyDPadBuff &= ~PAD_KEYMASK_DPAD_LEFT;
	}

	if( !digitalRead( RPI_GPIO_RIGHT ))
	{
		gKeyDPadBuff |= PAD_KEYMASK_DPAD_RIGHT;
	} else {
		gKeyDPadBuff &= ~PAD_KEYMASK_DPAD_RIGHT;
	}

	gKeyBuff = 0;
	if( !digitalRead( RPI_GPIO_START )) gKeyBuff |= PAD_KEYMASK_START;
	if( !digitalRead( RPI_GPIO_BTN0 )) gKeyBuff |= PAD_KEYMASK_PRIMARY;
	if( !digitalRead( RPI_GPIO_BTN1 )) gKeyBuff |= PAD_KEYMASK_SECONDARY;
	if( !digitalRead( RPI_GPIO_BTN2 )) gKeyBuff |= PAD_KEYMASK_SELECT;
#endif
	gkeys <<= 4;
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
	if( HasBit( gKeyDPadBuff, PAD_KEYMASK_DPAD_LEFT ))	ret -= 1.0f;
	if( HasBit( gKeyDPadBuff, PAD_KEYMASK_DPAD_RIGHT ))	ret += 1.0f;
	
	return ret;
}

float padGetY()
{
	float ret = 0;
	if( HasBit( gKeyDPadBuff, PAD_KEYMASK_DPAD_UP ))	ret -= 1.0f;
	if( HasBit( gKeyDPadBuff, PAD_KEYMASK_DPAD_DOWN ))	ret += 1.0f;
	
	return ret;
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
