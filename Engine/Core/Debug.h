//
//  Debug.h
//  TinyShooter
//
//  Created by Magnus Runesson on 2016-02-09.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#ifndef Debug_h
#define Debug_h

void debugInit();

#ifdef ENGINE_TARGET_RPI_CIRCLE

extern void circleLog( char* _pszFormat, ... );
extern void circleLog( const char* _pszFormat, ... );

//#define debugLog {}
#define debugLog(...) circleLog(__VA_ARGS__)

#else

void debugLog( const char* _pszFormat, ... );

#endif // ENGINE_TARGET_RPI_CIRCLE

#endif /* Debug_h */
