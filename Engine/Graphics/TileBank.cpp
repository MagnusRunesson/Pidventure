//
//  TileBank.cpp
//  TinyArcade
//
//  Created by Magnus Runesson on 2015-12-27.
//  Copyright © 2015 Magnus Runesson. All rights reserved.
//

#include "Engine/Util/String.h"
#include "Engine/Graphics/TileBank.h"
#include "Engine/IO/File.h"
#include "Engine/Core/Debug.h"
#include "Engine/Core/Memory.h"

static char g_pszImageFullFileName[ 1024 ];

CTileBank* tilebankLoad(const char* _pszFileName)
{
	stringCombine( g_pszImageFullFileName, 1024, _pszFileName, ".petb" );
	void* pData;
	int readBytes;
	if( fileLoad( g_pszImageFullFileName, &pData, &readBytes ) == false)
	{
		debugLog("Failed to load tilebank '%s.petb'\n", _pszFileName);
		return NULL;
	}
	
	return (CTileBank*)pData;
}

void tilebankUnload( CTileBank* _pszImage )
{
	fileUnload( _pszImage );
}
