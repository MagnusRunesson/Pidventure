//
//  file_osx.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-06.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include "Engine/Platform/RPi_Circle/main.h"
#include "Engine/Util/String.h"
#include "Engine/IO/File.h"
#include "Engine/IO/FileCache.h"
#include "Engine/Core/Debug.h"
#include "Engine/Core/Memory.h"

extern bool circleFileLoad(const char* _pszFileName, void** _ppReadData, int* _pReadBytes);

const char* pszBasePath = "";
char g_pszFileName[ 1024 ];

char* fileGetFullName( const char* _pszFileName )
{
	stringCombine( g_pszFileName, 1024, pszBasePath, fileTranslatorGetCrunchedName( _pszFileName ));
	return g_pszFileName;
}

bool fileLoad(const char* _pszFileName, void** _ppReadData, int* _pReadBytes)
{
	if( fileCacheGetFile( _pszFileName, _ppReadData, _pReadBytes ))
		return true;
	
	//
	// File didn't exist in cache
	//
//	debugLog( "Loading file '%s'\n", _pszFileName );
	const char* pszFullFileName = fileGetFullName( _pszFileName );
//	debugLog( "Complete crunched name '%s'\n", pszFullFileName );
	bool bSuccess = circleFileLoad( pszFullFileName, _ppReadData, _pReadBytes );

	/*
	FILE* f = fopen(pszFullFileName, "rb");
	debugLog( "Opened file: 0x%08x\n", f );
	if(f == NULL)
	{
		printf( "Failed to open file '%s'\n", pszFullFileName );
		return false;
	}
	
	// Get file size
	fseek( f, 0, SEEK_END );
	long fileSize = ftell( f );
	fseek( f, 0, SEEK_SET );
	debugLog( "File size: %i\n", fileSize );
	
	*_ppReadData = new char[ fileSize ];
	debugLog( "Allocated data: 0x%08x\n", *_ppReadData );
	*_pReadBytes = (int)fread( *_ppReadData, 1, fileSize, f );
	debugLog( "Read %i bytes\n", *_pReadBytes );
	
	fclose( f );
	debugLog("Done\n");
	*/
	
	fileCacheAddFile( _pszFileName, *_ppReadData, *_pReadBytes );

	return true;
}

void fileUnload( void* _ptr )
{
	fileCacheUnreferenceFile( _ptr );
}
