//
//  filestream_rpi_circle.cpp
//  Pidventure_Circle
//
//  Created by Magnus Runesson on 2017-09-06.
//  Copyright © 2017 Magnus Runesson. All rights reserved.
//

#include "Engine/stdc/stdc.h"
#include "Engine/Core/Debug.h"
#include "Engine/IO/FileStream.h"
#include "Engine/Platform/RPi_Circle/filestream_rpi_circle.h"
#include "Engine/Platform/RPi_Circle/main.h"

extern CApp* g_pApp;

class CFileStream
{
public:
	bool isUsed;
	uint32 firstSector;		// First sector on disk where this stream lives
	uint32 lastSector;		// So we know when we've reached the end
	uint32 nextSector;		// Next sector to be read
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
	//debugLog( "fso 1" );
	g_pApp->m_FileSystem.m_FileTableLock.Acquire ();
	
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
		g_pApp->m_FileSystem.m_FileTableLock.Release ();

		debugLog( "fileStreamOpen, out of streams\n" );
		return -1;
	}
	
	//debugLog( "fso 3" );

	assert( _pszFileName != 0 );
	TFATDirectoryEntry* pEntry = g_pApp->m_FileSystem.m_Root.GetEntry( _pszFileName );
	if( pEntry == NULL )
	{
		g_pApp->m_FileSystem.m_FileTableLock.Release ();

		debugLog( "fileStreamOpen, failed to open file '%s'\n", _pszFileName );

		return -1;
	}

	//debugLog( "fso 4" );

	uint32 cluster = (uint32)( pEntry->nFirstClusterHigh << 16 | pEntry->nFirstClusterLow );
	
	// Find a free stream to use
	CFileStream* pStream = &g_fileStreams[ streamHandle ];
	
	// Setup stream entry
	pStream->isUsed = true;
	pStream->fileSizeBytes = pEntry->nFileSize;
	pStream->firstSector = g_pApp->m_FileSystem.m_FATInfo.GetFirstSector( cluster );
	pStream->lastSector = pStream->firstSector + ((pStream->fileSizeBytes + 511) / 512);
	pStream->nextSector = pStream->firstSector;
	
	//debugLog("fsopen. sb=%i, fs=%i, ls=%i\n", pStream->fileSizeBytes, pStream->firstSector, pStream->lastSector );

	g_pApp->m_FileSystem.m_Root.FreeEntry( false );

	g_pApp->m_FileSystem.m_FileTableLock.Release();

	return streamHandle;
}

void fileStreamClose( int _fileStreamHandle )
{
	g_fileStreams[ _fileStreamHandle ].isUsed = false;
}

void fileStreamRewind( int _fileStreamHandle )
{
	g_fileStreams[ _fileStreamHandle ].nextSector = g_fileStreams[ _fileStreamHandle ].firstSector;
}

//
// Reads a chunk, or less if it is the last chunk and the file size
// isn't a multiple of STREAM_CHUNK_SIZE. Return the number of BYTES
// read, so most of the time STREAM_CHUNK_SIZE will be returned.
//
int fileStreamReadNextChunk( int _fileStreamHandle, void* _pReadDestination )
{
	g_pApp->m_FileSystem.m_FileTableLock.Acquire ();

	CFileStream* pStream = &g_fileStreams[ _fileStreamHandle ];
	if( pStream->nextSector > pStream->lastSector )
	{
		g_pApp->m_FileSystem.m_FileTableLock.Release();
		return 0;	// We have already read past the end
	}

	// Seek to the right sector on the SD card
	uint64 seekOffset = pStream->nextSector;
	seekOffset *= STREAM_CHUNK_SIZE;
	g_pApp->m_FileSystem.m_Cache.m_DiskLock.Acquire();
	g_pApp->m_FileSystem.m_Cache.m_pPartition->Seek( seekOffset );
	
	// Read one sector from the SD card
	int readJanne = g_pApp->m_FileSystem.m_Cache.m_pPartition->Read( _pReadDestination, 512 );
	
	//
	g_pApp->m_FileSystem.m_Cache.m_DiskLock.Release();
	//debugLog("readJanne=%i\n", readJanne );
	
	int readBytes = STREAM_CHUNK_SIZE;
	if( pStream->nextSector == pStream->lastSector )
	{
		// We just read from the last sector. That means we might not return a full 512 bytes.
		readBytes = (pStream->fileSizeBytes & 511);
	}

	//
	g_pApp->m_FileSystem.m_FileTableLock.Release();
	
	//
	pStream->nextSector++;
	
	//
	return readBytes;
}
