//
//  FileStream.h
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-06.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#ifndef __FileStream_h
#define __FileStream_h

#define MAX_STREAMS					(10)
#define STREAM_CHUNK_SIZE			(512)	// Should match the size of the minimum readable unit from the SD card on the Raspberry Pi, for performance reasons
#define FILESTREAM_INVALID_HANDLE	(-1)


void fileStreamInit();
int fileStreamOpen( const char* _pszFileName );
void fileStreamClose( int _fileStreamHandle );
void fileStreamRewind( int _fileStreamHandle );

//
// Reads a chunk, or less if it is the last chunk and the file size
// isn't a multiple of STREAM_CHUNK_SIZE. Return the number of BYTES
// read, so most of the time STREAM_CHUNK_SIZE will be returned.
//
int fileStreamReadNextChunk( int _fileStreamHandle, void* _pReadDestination );

#endif // __FileStream_h
