//
//  Timer.cpp
//  TinyShooter
//
//  Created by Magnus Runesson on 2016-01-16.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

//#include "SDL2/SDL.h"
#ifndef ENGINE_TARGET_RPI_CIRCLE
#include <chrono>
#endif

#include "Engine/Core/Timer.h"
#include "Engine/Core/Types.h"

uint32 timerStartMicros;
uint32 timerStartMillis;

float timeDelta()
{
	return 1.0f / 60.0f;	// Pretend we're running at 60 fps
}

uint32 micros()
{
#ifdef ENGINE_TARGET_RPI_CIRCLE
	return 0;
#else
	auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
	uint32 ret = (uint32)std::chrono::duration_cast<std::chrono::microseconds>(now).count();
	return ret - timerStartMicros;
#endif
}

uint32 millis()
{
#ifdef ENGINE_TARGET_RPI_CIRCLE
	return 0;
#else
	auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
	uint32 ret = (uint32)std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
	return ret - timerStartMillis;
#endif
}

uint32 nanos()
{
#ifdef ENGINE_TARGET_RPI_CIRCLE
	return 0;
#else
	auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
	uint32 ret = (uint32)std::chrono::duration_cast<std::chrono::nanoseconds>( now ).count();
	return ret;
#endif
}

/*
void delay( uint32 milliseconds )
{
	SDL_Delay( milliseconds );
}

void delayMicroseconds( uint32 microseconds )
{
	delay( microseconds / 1000 );
}
 */

void timerInit()
{
	timerStartMicros = 0;
	timerStartMicros = micros();
	
	timerStartMillis = 0;
	timerStartMillis = millis();
}

