//
//  filestream_rpi_circle.cpp
//  Pidventure_Circle
//
//  Created by Magnus Runesson on 2017-09-06.
//  Copyright © 2017 Magnus Runesson. All rights reserved.
//

#include "Engine/stdc/stdc.h"
#include "Engine/Core/Debug.h"
#include "Engine/IO/File.h"
#include "Engine/IO/FileStream.h"
#include "Engine/Platform/RPi_Circle/filestream_rpi_circle.h"
#include "Engine/Platform/RPi_Circle/main.h"

extern CApp* g_pApp;

class CFileStream
{
public:
	bool isUsed;
	unsigned hFile;
	uint32 fileSizeBytes;
	/*
	uint32 firstSector;		// First sector on disk where this stream lives
	uint32 lastSector;		// So we know when we've reached the end
	uint32 nextSector;		// Next sector to be read
	uint32 fileSizeBytes;	// Because file sizes probably is a multiple of 512 (sector size) ver rarely
	 */
};

CFileStream g_fileStreams[ MAX_STREAMS ];

void fileStreamInit()
{
	int i;
	for( i=0; i<MAX_STREAMS; i++ )
	{
		g_fileStreams[ i ].isUsed = false;
	}
}

int fileStreamOpen( const char* _pszFileName )
{
	int streamHandle;
	for( streamHandle=0; streamHandle<=MAX_STREAMS; streamHandle++ )
	{
		if( !g_fileStreams[ streamHandle ].isUsed )
		{
			break;
		}
	}
	
	if( streamHandle > MAX_STREAMS )
	{
		debugLog( "fileStreamOpen, out of streams\n" );
		return FILESTREAM_INVALID_HANDLE;
	}
	
	const char* pszCrunchedName = fileGetFullNameLoad( _pszFileName );

	//
	CFileStream* pStream = &g_fileStreams[ streamHandle ];
	pStream->hFile = g_pApp->m_FileSystem.FileOpen( pszCrunchedName );
	pStream->fileSizeBytes = g_pApp->m_FileSystem.FileSize( pStream->hFile );
	pStream->isUsed = true;

	//
	return streamHandle;
}

void fileStreamClose( int _fileStreamHandle )
{
	CFileStream* pStream = &g_fileStreams[ _fileStreamHandle ];
	if( pStream->isUsed )
	{
		if( !g_pApp->m_FileSystem.FileClose( pStream->hFile ))
		{
			debugLog("Cannot close file");
		}
	}
	
	pStream->isUsed = false;
}

void fileStreamRewind( int _fileStreamHandle )
{
	CFileStream* pStream = &g_fileStreams[ _fileStreamHandle ];
	g_pApp->m_FileSystem.FileRewind( pStream->hFile );
}

//
// Reads a chunk, or less if it is the last chunk and the file size
// isn't a multiple of STREAM_CHUNK_SIZE. Return the number of BYTES
// read, so most of the time STREAM_CHUNK_SIZE will be returned.
//
int fileStreamReadNextChunk( int _fileStreamHandle, void* _pReadDestination )
{
	CFileStream* pStream = &g_fileStreams[ _fileStreamHandle ];
	
	unsigned nResult = g_pApp->m_FileSystem.FileRead( pStream->hFile, _pReadDestination, STREAM_CHUNK_SIZE );
	
	if( nResult == FS_ERROR )
	{
		debugLog("Read error on stream\n");
		return 0;
	}

	return nResult;
}
