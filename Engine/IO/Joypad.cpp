//
//  Joypad.cpp
//  TinyArcade
//
//  Created by Magnus Runesson on 2015-12-03.
//  Copyright © 2015 Magnus Runesson. All rights reserved.
//

#include "Engine/Core/Types.h"
#include "Engine/IO/Joypad.h"
#include "Engine/Core/BitHelpers.h"
#include "Engine/Core/Debug.h"

extern void padHardwareInit();
extern void padHardwareUpdate();

#define BUFFER_SWITCH_LENGTH	(16)
#define BUFFER_SWITCH_MASK		((1<<BUFFER_SWITCH_LENGTH)-1)

uint32 gkeys;
uint32 gKeyBuff;

void padInit()
{
	gkeys = 0;
	gKeyBuff = 0;

	padHardwareInit();
}

void padUpdate()
{
	padHardwareUpdate();
	
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
