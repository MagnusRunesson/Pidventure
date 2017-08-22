//
//  TileMap.cpp
//  TinyArcade
//
//  Created by Magnus Runesson on 2016-01-03.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include "Engine/Util/String.h"
#include "Engine/Graphics/TileMap.h"
#include "Engine/IO/File.h"
#include "Engine/Core/Debug.h"
#include "Engine/Core/Memory.h"

static char g_pszImageFullFileName[ 1024 ];

CTileMap* tilemapLoad( const char* _pszFileName )
{
	stringCombine( g_pszImageFullFileName, 1024, _pszFileName, ".petm" );
	void* pData;
	int readBytes;
	if( fileLoad( g_pszImageFullFileName, &pData, &readBytes ) == false)
	{
		debugLog("Failed to load tilebank '%s.petm'\n", _pszFileName);
		return NULL;
	}
	
	return (CTileMap*)pData;
}

void tilemapUnload( CTileMap* _pszImage )
{
	fileUnload( _pszImage );
}
