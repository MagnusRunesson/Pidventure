//
//  file_osx.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-06.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include <stdio.h>
#include <memory.h>
#include "Engine/IO/File.h"

const char* pszBasePath = "/Users/magnusrunesson/Projects/Pidventure/Pidventure/data/";
char g_pszFileName[ 1024 ];

char* fileGetFullName(const char* _pszFileName)
{
	snprintf(g_pszFileName, 1024, "%s%s", pszBasePath, _pszFileName);
	return g_pszFileName;
}

bool fileLoad(const char* _pszFileName, void** _ppReadData, int* _pReadBytes)
{
	const char* pszFullFileName = fileGetFullName( _pszFileName );
	FILE* f = fopen(pszFullFileName, "rb");
	if(f == NULL)
		return false;
	
	// Get file size
	fseek( f, 0, SEEK_END );
	long fileSize = ftell( f );
	fseek( f, 0, SEEK_SET );
	
	*_ppReadData = new char[ fileSize ];
	*_pReadBytes = (int)fread( *_ppReadData, 1, fileSize, f );
	
	fclose( f );
	
	return true;
}

bool fileLoad(const char* _pszFileName, void* _pReadDestination, int _bufferSize, int* _pReadBytes)
{
	const char* pszFullFileName = fileGetFullName( _pszFileName );
	FILE* f = fopen(pszFullFileName, "rb");
	if(f == NULL)
		return false;
	
	// Get file size
	fseek( f, 0, SEEK_END );
	long fileSize = ftell( f );
	fseek( f, 0, SEEK_SET );

	if( fileSize > _bufferSize )
	{
		fclose( f );
		return false;
	}
	
	*_pReadBytes = (int)fread( _pReadDestination, 1, fileSize, f );
	
	fclose( f );
	
	return true;
}
