//
//  Image.cpp
//  TinyArcade
//
//  Created by Magnus Runesson on 2015-12-03.
//  Copyright © 2015 Magnus Runesson. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include "Engine/Graphics/Image.h"
#include "Engine/IO/File.h"

char g_pszImageFullFileName[ 1024 ];

Image* imageLoad(const char* _pszFileName)
{
	snprintf( g_pszImageFullFileName, 1024, "%s.pei", _pszFileName );
	void* pData;
	int readBytes;
	if( fileLoad( g_pszImageFullFileName, &pData, &readBytes ) == false)
		return NULL;
	
	return (Image*)pData;
}
