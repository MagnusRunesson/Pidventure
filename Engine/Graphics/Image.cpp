//
//  Image.cpp
//  TinyArcade
//
//  Created by Magnus Runesson on 2015-12-03.
//  Copyright © 2015 Magnus Runesson. All rights reserved.
//

#include "Engine/stdc/stdlib.h"
#include "Engine/Util/String.h"
#include "Engine/Graphics/Image.h"
#include "Engine/IO/File.h"
#include "Engine/Core/Debug.h"
#include "Engine/Core/Memory.h"

static char g_pszImageFullFileName[ 1024 ];

Image* imageLoad(const char* _pszFileName)
{
	stringCombine( g_pszImageFullFileName, 1024, _pszFileName, ".pei" );
	void* pData;
	int readBytes;
	if( fileLoad( g_pszImageFullFileName, &pData, &readBytes ) == false)
	{
		debugLog("Failed to load image '%s.pei'\n", _pszFileName);
		return NULL;
	}
	
	return (Image*)pData;
}

void imageUnload( const Image* _pszImage )
{
	fileUnload( (void*)_pszImage );
}

void imageUnload( Image* _pszImage )
{
	fileUnload( _pszImage );
}
