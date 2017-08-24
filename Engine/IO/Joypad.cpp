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

uint32 g_JoypadVirtualKeysBuffer;
uint32 g_JoypadHardwareBuffer;

void padInit()
{
	g_JoypadVirtualKeysBuffer = 0;
	g_JoypadHardwareBuffer = 0;

	padHardwareInit();
}

void padUpdate()
{
	padHardwareUpdate();
	
	g_JoypadVirtualKeysBuffer <<= BUFFER_SWITCH_LENGTH;
	g_JoypadVirtualKeysBuffer |= g_JoypadHardwareBuffer;
}

uint8 padGet( float* _x, float* _y )
{
	*_x = padGetX();
	*_y = padGetY();

	return g_JoypadVirtualKeysBuffer;
}

uint8 padGetKeys()
{
	return g_JoypadVirtualKeysBuffer;
}

float padGetX()
{
	float ret = 0;
	if( HasBit( g_JoypadVirtualKeysBuffer, PAD_KEYMASK_DPAD_LEFT ))	ret -= 1.0f;
	if( HasBit( g_JoypadVirtualKeysBuffer, PAD_KEYMASK_DPAD_RIGHT ))	ret += 1.0f;
	
	return ret;
}

float padGetY()
{
	float ret = 0;
	if( HasBit( g_JoypadVirtualKeysBuffer, PAD_KEYMASK_DPAD_UP ))	ret -= 1.0f;
	if( HasBit( g_JoypadVirtualKeysBuffer, PAD_KEYMASK_DPAD_DOWN ))	ret += 1.0f;
	
	return ret;
}

uint16 padGetPressed()
{
	uint16 prev = (g_JoypadVirtualKeysBuffer>>BUFFER_SWITCH_LENGTH) & BUFFER_SWITCH_MASK;
	uint16 curr = (g_JoypadVirtualKeysBuffer) & BUFFER_SWITCH_MASK;
	uint16 ret = (prev ^ curr) & curr;
	return ret;
}

uint16 padGetReleased()
{
	uint16 prev = (g_JoypadVirtualKeysBuffer>>BUFFER_SWITCH_LENGTH) & BUFFER_SWITCH_MASK;
	uint16 curr = (g_JoypadVirtualKeysBuffer) & BUFFER_SWITCH_MASK;
	uint16 ret = (prev ^ curr) & prev;
	return ret;
}
