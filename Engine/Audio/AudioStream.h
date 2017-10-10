//
//  AudioStream.h
//  TinyShooter
//
//  Created by Magnus Runesson on 2016-02-24.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#ifndef AudioStream_h
#define AudioStream_h

#include "Engine/Core/Types.h"
#include "Engine/IO/FileStream.h"

#define STREAM_BUFFER_SIZE (STREAM_CHUNK_SIZE)

class AudioStream
{
public:
	sint8 m_streamBufferA[ STREAM_BUFFER_SIZE ];
	sint8 m_streamBufferB[ STREAM_BUFFER_SIZE ];

	uint8 m_lastStreamBufferPage = 1;					// The page we streamed from last time we read file data
	uint8 m_currentStreamBufferPage = 0;				// The page we are currently streaming from
	uint16 m_currentStreamBufferIndex = 0;				// The byte offset in a page we are currently streaming from
	sint8* m_currentStreamBuffer = m_streamBufferA;		// Shortcut to the current page to stream from
	sint8* m_streamBuffers[ 2 ] =
	{
		m_streamBufferA,
		m_streamBufferB
	};

	bool m_isPlaying;
	
	AudioStream();
	void Update();
	
	void Reset();
	void OpenStream( const char* _pszFileName );
	void CloseStream();
	void Rewind();
	void Play();
	void Pause();
	sint8 GetNextSample();
	void SetVolume( float _volume );
	
	int m_fileStreamHandle;
	
private:
	void StreamIntoBuffer( sint8* _pBuffer );
	void ResetPlayback();
	void ResetStream();
	
	float m_volume;
};

#endif /* AudioStream_h */
