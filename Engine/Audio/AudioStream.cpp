//
//  AudioStream.cpp
//  TinyShooter
//
//  Created by Magnus Runesson on 2016-02-24.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include "Engine/Core/Debug.h"
#include "Engine/Util/String.h"
#include "Engine/IO/FileStream.h"
#include "Engine/Audio/AudioStream.h"

#define SWAP_PAGE( _a ){ _a=(_a==0)?1:0; }



AudioStream::AudioStream()
{
	Reset();
}

void AudioStream::Reset()
{
	ResetInternals();
	CloseStream();
}

void AudioStream::OpenStream( const char *_pszFileName )
{
	Reset();

	char buffe[ 1024 ];
	stringCombine( buffe, 1024, _pszFileName, ".raw" );
	
	m_fileStreamHandle = fileStreamOpen( buffe );
	if( m_fileStreamHandle != FILESTREAM_INVALID_HANDLE )
	{
		StreamIntoBuffer( m_streamBufferA );
		StreamIntoBuffer( m_streamBufferB );
	}
}

void AudioStream::CloseStream()
{
	Pause();
	fileStreamClose( m_fileStreamHandle );
	m_fileStreamHandle = FILESTREAM_INVALID_HANDLE;
}

void AudioStream::Rewind()
{
	ResetInternals();
	fileStreamRewind( m_fileStreamHandle );
}

void AudioStream::Play()
{
	m_isPlaying = true;
}

void AudioStream::Pause()
{
	m_isPlaying = false;
}

void AudioStream::Update()
{
	// Wait until we started streaming from the last page we read
	if( m_lastStreamBufferPage != m_currentStreamBufferPage )
		return;
	
	// At this point the audio playback have started streaming from
	// the last read page, so we need to quickly load another page
	SWAP_PAGE( m_lastStreamBufferPage );
	//m_readAheads = 0;

	//debugLog("Filling buffer %i\n", m_lastStreamBufferPage );
	uint32 readBytes = StreamIntoBuffer( m_streamBuffers[ m_lastStreamBufferPage ]);
	if( readBytes != STREAM_BUFFER_SIZE )
	{
		// We've read past the end of file. But we may still have read
		// samples that should be played so we can't just pause right
		// now. However, if we've read 0 bytes there are no more samples
		// left to play, yay!
		if( readBytes == 0 )
			Pause();
	}
}

sint8 AudioStream::GetNextSample()
{
	sint8 ret = m_currentStreamBuffer[ m_currentStreamBufferIndex ];
	
	//
	m_currentStreamBufferIndex++;
	if( m_currentStreamBufferIndex >= STREAM_BUFFER_SIZE )
	{
		// Reset from the beginning
		m_currentStreamBufferIndex = 0;

		//debugLog("Played end of buffer %i!\n", m_currentStreamBufferPage );

		// Swap which buffer to play from
		SWAP_PAGE( m_currentStreamBufferPage );
		m_currentStreamBuffer = m_streamBuffers[ m_currentStreamBufferPage  ];

		// Load more data yo!
		Update();
	}
	
	return ret;
}

uint32 AudioStream::StreamIntoBuffer( sint8* _pBuffer )
{
	uint32 readBytes = 0;
	int i;
	for( i=0; i<STREAM_CHUNKS_PER_BUFFER; i++ )
		readBytes += fileStreamReadNextChunk( m_fileStreamHandle, _pBuffer + (i*STREAM_CHUNK_SIZE));
	
	return readBytes;
}

void AudioStream::ResetInternals()
{
	m_lastStreamBufferPage = 1;						// The page we streamed from last time we read file data
	m_currentStreamBufferPage = 0;					// The page we are currently streaming from
	m_currentStreamBufferIndex = 0;					// The byte offset in a page we are currently streaming from
	m_currentStreamBuffer = m_streamBufferA;		// Shortcut to the current page to stream from
	m_streamBuffers[ 0 ] = m_streamBufferA;
	m_streamBuffers[ 1 ] = m_streamBufferB;
	m_isPlaying = false;
}
