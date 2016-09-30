//
//  TileMap.cpp
//  TinyArcade
//
//  Created by Magnus Runesson on 2016-01-03.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include <stdio.h>
#include "Engine/Graphics/TileMap.h"
#include "Engine/IO/File.h"
#include "Engine/Core/Debug.h"

static char g_pszImageFullFileName[ 1024 ];

CTileMap* tilemapLoad( const char* _pszFileName )
{
	snprintf( g_pszImageFullFileName, 1024, "%s.pest", _pszFileName );
	void* pData;
	int readBytes;
	if( fileLoad( g_pszImageFullFileName, &pData, &readBytes ) == false)
	{
		debugLog("Failed to load tilebank '%s.petb'\n", _pszFileName);
		return NULL;
	}
	
	return (CTileMap*)pData;
}

void tilemapUnload( CTileMap* _pszImage )
{
	delete _pszImage;
}
