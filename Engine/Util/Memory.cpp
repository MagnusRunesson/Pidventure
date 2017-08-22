//
//  Memory.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2017-08-21.
//  Copyright © 2017 Magnus Runesson. All rights reserved.
//

#include "Memory.h"

void memorySet( void* _pDestination, int _value, int _nBytes )
{
	char* pDest = (char*)_pDestination;
	
	int i;
	for( i=0; i<_nBytes; i++ )
		pDest[ i ] = _value;
}
