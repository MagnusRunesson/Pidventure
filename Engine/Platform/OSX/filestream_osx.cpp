//
//  filestream_osx.cpp
//  Engine
//
//  Created by Magnus Runesson on 2017-10-04.
//  Copyright © 2017 Magnus Runesson. All rights reserved.
//

#include <stdio.h>
#include "Engine/Core/Types.h"
#include "Engine/Core/Debug.h"
#include "Engine/IO/File.h"
#include "Engine/IO/Filestream.h"

class CFileStream
{
public:
	bool isUsed;
	FILE* hFile;
	uint32 fileSizeBytes;	// Because file sizes probably is a multiple of 512 (sector size) ver rarely
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
	
	//debugLog( "fso 2" );
	
	if( streamHandle > MAX_STREAMS )
	{
		debugLog( "fileStreamOpen, out of streams\n" );
		return FILESTREAM_INVALID_HANDLE;
	}

	const char* pszCrunchedName = fileGetFullNameLoad( _pszFileName );
	
	CFileStream* pStream = &g_fileStreams[ streamHandle ];
	FILE* f = fopen( pszCrunchedName, "r" );
	fseek( f, 0, SEEK_END );
	pStream->fileSizeBytes = (uint32)ftell( f );
	pStream->hFile = f;
	pStream->isUsed = true;
	
	fseek( f, 0, SEEK_SET );
	
	return streamHandle;
}

void fileStreamClose( int _fileStreamHandle )
{
	CFileStream* pStream = &g_fileStreams[ _fileStreamHandle ];
	fclose( pStream->hFile );
	pStream->isUsed = false;
	pStream->hFile = NULL;
}

void fileStreamRewind( int _fileStreamHandle )
{
	CFileStream* pStream = &g_fileStreams[ _fileStreamHandle ];
	fseek( pStream->hFile, 0, SEEK_SET );
}

//
// Reads a chunk. Always return STREAM_CHUNK_SIZE bytes, so
// the stream will rewind and start over when EOF is reached.
//
void fileStreamReadNextChunk( int _fileStreamHandle, void* _pReadDestination )
{
	CFileStream* pStream = &g_fileStreams[ _fileStreamHandle ];
	int filePosition = (int)ftell( pStream->hFile );
	int bytesLeft = pStream->fileSizeBytes - filePosition;
	if( bytesLeft == 0 ) {
		fseek( pStream->hFile, 0, SEEK_SET );
		fread( _pReadDestination, 1, STREAM_CHUNK_SIZE, pStream->hFile );
	} else if( bytesLeft < STREAM_CHUNK_SIZE )
	{
		int a = (int)fread( _pReadDestination, 1, bytesLeft, pStream->hFile );
		fseek( pStream->hFile, 0, SEEK_SET );
		int wb = STREAM_CHUNK_SIZE - bytesLeft;
		int b = (int)fread( (void*)(((unsigned char*)_pReadDestination) + bytesLeft), 1, wb, pStream->hFile );
		/*
		debugLog("Bytes left: %i. Read from end: %i, read from start: %i, wanted read from start: %i, is this on: %i\n", bytesLeft, a, b, wb, 25 );
		FILE* fo = fopen( "teststreamloop.bin", "wb" );
		fwrite( _pReadDestination, 1, STREAM_CHUNK_SIZE, fo );
		fclose( fo );
		 */
	} else {
		fread( _pReadDestination, 1, STREAM_CHUNK_SIZE, pStream->hFile );
	}
}
