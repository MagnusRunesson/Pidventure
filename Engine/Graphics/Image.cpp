//
//  Image.cpp
//  TinyArcade
//
//  Created by Magnus Runesson on 2015-12-03.
//  Copyright © 2015 Magnus Runesson. All rights reserved.
//

#include <stdlib.h>
#include "Engine/Graphics/Image.h"
#include "Engine/IO/File.h"

Image* imageLoad(const char* _pszFile)
{
	void* pData;
	int readBytes;
	if(fileLoad(_pszFile, &pData, &readBytes) == false)
		return NULL;
	
	return (Image*)pData;
}
