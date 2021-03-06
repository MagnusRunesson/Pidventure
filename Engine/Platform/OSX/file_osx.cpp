#define FILE_TRACKING	(0)

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
#include "Engine/IO/FileCache.h"
#include "Engine/Core/Debug.h"
#include "Engine/Core/Memory.h"

#if FILE_TRACKING == 1

static FILE* g_fileNew = NULL;
static FILE* g_fileDelete = NULL;

void fileTrackInit()
{
	g_fileNew = fopen("filetrack_open.txt", "w");
	g_fileDelete = fopen("filetrack_close.txt", "w");
}

void fileTrackExit()
{
	fclose(g_fileNew);
	fclose(g_fileDelete);
}

void fileTrackOpen(void* _ptr, const char* _pszFileName, size_t _fileSize)
{
	if(g_fileNew == NULL)
		fileTrackInit();
	fprintf( g_fileNew, "0x%016llx,%s,%u\n", (unsigned long long)_ptr, _pszFileName, (unsigned int)_fileSize );
	//debugLog("------------------> Calling new from %s:%i - returning 0x%08x\n", _pszFileName, 0, _ptr );
}

void fileTrackClose(void* _ptr)
{
	if(g_fileNew == NULL)
		fileTrackInit();
	fprintf( g_fileDelete, "0x%016llx\n", (unsigned long long)_ptr );
}

#else

#define fileTrackOpen(a,b,c)
#define fileTrackClose(a)

#endif


const char* pszBasePathLoad = "./data/";
const char* pszBasePathSave = "./persistent/";
char g_pszFileName[ 1024 ];

char* fileGetFullNameLoad(const char* _pszFileName)
{
	snprintf(g_pszFileName, 1024, "%s%s", pszBasePathLoad, fileTranslatorGetCrunchedName( _pszFileName ));
	return g_pszFileName;
}

char* fileGetFullNameSave(const char* _pszFileName)
{
	snprintf(g_pszFileName, 1024, "%s%s", pszBasePathSave, _pszFileName );
	return g_pszFileName;
}

bool fileLoad(const char* _pszFileName, void** _ppReadData, int* _pReadBytes)
{
	if( fileCacheGetFile( _pszFileName, _ppReadData, _pReadBytes ))
		return true;

	//
	// File didn't exist in log
	//
	const char* pszFullFileName = fileGetFullNameLoad( _pszFileName );
	FILE* f = fopen(pszFullFileName, "rb");
	//debugLog( "Opened file: 0x%08x by name '%s'\n", f, pszFullFileName );
	if(f == NULL)
		return false;

	// Get file size
	fseek( f, 0, SEEK_END );
	long fileSize = ftell( f );
	fseek( f, 0, SEEK_SET );
	//debugLog( "File size: %i\n", fileSize );
	
	if(fileSize == 0)
	{
		// The RPi implementation of fileLoad fail if filesize is 0 so we do that on OSX aswell
		fclose(f);
		return false;
	}
	
	*_ppReadData = new char[ fileSize ];
	//debugLog( "Allocated data: 0x%08x\n", *_ppReadData );
	*_pReadBytes = (int)fread( *_ppReadData, 1, fileSize, f );
	//debugLog( "Read %i bytes\n", *_pReadBytes );
	
	fclose( f );
	//debugLog("Done\n");
	
	fileTrackOpen( *_ppReadData, _pszFileName, *_pReadBytes );
	fileCacheAddFile( _pszFileName, *_ppReadData, *_pReadBytes );
	
	return true;
}

void fileUnload( void* _ptr )
{
	fileTrackClose( _ptr );
	fileCacheUnreferenceFile( _ptr );
}

void fileSave( const char* _pszFileName, void* _pContent, int _contentLength )
{
	const char* pszFullFileName = fileGetFullNameSave( _pszFileName );
	FILE* f = fopen( pszFullFileName, "w" );
	if(f == NULL)
		return;
	
	fwrite( _pContent, 1, _contentLength, f );
	fclose( f );
}
