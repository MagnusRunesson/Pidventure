//
//  Debug.cpp
//  TinyShooter
//
//  Created by Magnus Runesson on 2016-02-09.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#ifndef ENGINE_TARGET_RPI_CIRCLE
#include <stdio.h>
#include <stdarg.h>
#include "Engine/Core/Debug.h"
#endif

void debugInit()
{
}

void debugLog( const char* _pszFormat, ... )
{
#ifndef ENGINE_TARGET_RPI_CIRCLE
	const int SIZE = 256;
	char buffer[ SIZE ];
	va_list args;
	va_start( args, _pszFormat );
	
	vsnprintf( buffer, SIZE-1, _pszFormat, args );
	printf( "%s", buffer );
	
	va_end( args );
#endif
}
