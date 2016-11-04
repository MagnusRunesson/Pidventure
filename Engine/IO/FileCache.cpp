//
//  FileCache.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2016-11-04.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "Engine/IO/File.h"
#include "Engine/IO/FileCache.h"
#include "Engine/Core/Debug.h"

const int FILE_CACHE_SIZE = 256;

class CCachedFile
{
public:
	CCachedFile()
	{
		NumReferences = 0;

		pszFileName = NULL;
		FileSize = 0;
		pFileData = NULL;
	}
	
	int NumReferences;
	
	const char* pszFileName;
	size_t FileSize;
	void* pFileData;
};

static CCachedFile fileCache[ FILE_CACHE_SIZE ];

CCachedFile* fileCacheGetCachedFileData( const char* _pszFileName )
{
	debugLog("FILE CACHE: Requested file '%s'\n", _pszFileName);
	
	int i;
	for( i=0; i<FILE_CACHE_SIZE; i++ )
	{
		CCachedFile* pFile = &fileCache[ i ];
		if( pFile->pszFileName == NULL )
			continue;
		
		if( strcmp( pFile->pszFileName, _pszFileName ) == 0 )
		{
			debugLog("FILE CACHE: Found file '%s'\n", pFile->pszFileName );
			return pFile;
		}
	}
	
	return NULL;
}

CCachedFile* fileCacheGetCachedFileData( void* _pData )
{
	int i;
	for( i=0; i<FILE_CACHE_SIZE; i++ )
	{
		CCachedFile* pFile = &fileCache[ i ];
		if( pFile->pszFileName == NULL )
			continue;
		
		if( pFile->pFileData == _pData )
			return pFile;
	}
	
	return NULL;
}

CCachedFile* fileCacheGetFreeEntry()
{
	int i;
	for( i=0; i<FILE_CACHE_SIZE; i++ )
	{
		CCachedFile* pFile = &fileCache[ i ];
		if( pFile->pszFileName == NULL )
			return pFile;
	}
	
	debugLog("ERROR: Oh noes! File cache is out of slots\n");
	return NULL;
}

bool fileCacheHaveFile( const char* _pszFileName )
{
	return fileCacheGetCachedFileData( _pszFileName ) != NULL;
}

bool fileCacheGetFile( const char* _pszFileName, void** _ppData, int* _pSize )
{
	CCachedFile* pFile = fileCacheGetCachedFileData( _pszFileName );
	if( pFile == NULL )
		return false;
	
	pFile->NumReferences++;
	
	*_ppData = pFile->pFileData;
	*_pSize = pFile->FileSize;
	
	return true;
}

void fileCacheUnreferenceFile( void* _pData )
{
	CCachedFile* pFile = fileCacheGetCachedFileData( _pData );
	if( pFile == NULL )
	{
		debugLog("ERROR: File cache was asked to unreference a file that doesn't exist in the file cache. Requested file memory ptr: '%s'\n", _pData );
		return;
	}
	
	pFile->NumReferences--;
}

void fileCacheAddFile( const char* _pszFileName, void* _pFileData, int _fileSize )
{
	CCachedFile* pFile = fileCacheGetFreeEntry();
	if( pFile == NULL )
		return;
	
	pFile->NumReferences = 1;
	pFile->pszFileName = strdup( _pszFileName );
	pFile->pFileData = _pFileData;
	pFile->FileSize = _fileSize;
}

void fileCacheReset()
{
	int i;
	for( i=0; i<FILE_CACHE_SIZE; i++ )
	{
		CCachedFile* pFile = &fileCache[ i ];
		if( pFile->pszFileName == NULL )
			continue;

		if(pFile->NumReferences)
			debugLog( "File '%s' have %i dangling references\n", pFile->pszFileName, pFile->NumReferences );

		delete[] (char*)pFile->pFileData;
		delete[] pFile->pszFileName;
		pFile->FileSize = 0;
		pFile->NumReferences = 0;
	}
}
