#include "Engine/Audio/AudioMixer.h"
#include "Engine/Platform/RPi_Circle/audio_rpi_circle.h"

#define SAMPLE_RATE     44100

CAudioRPiCircle::CAudioRPiCircle( CInterruptSystem *pInterrupt )
: CPWMSoundBaseDevice (pInterrupt, SAMPLE_RATE )
{
	assert ((1 << 12) <= GetRange () && GetRange () < (1 << 13));  // 12 bit range
}

CAudioRPiCircle::~CAudioRPiCircle( void )
{
}

unsigned CAudioRPiCircle::GetChunk( u32 *pBuffer, unsigned nChunkSize )
{
	float range = 2047.0f;
	
	uint32 sample = 0;
	while( sample < nChunkSize )
	{
		audioMixer.outputReadPosition++;
		if( audioMixer.outputReadPosition >= audioMixer.outputBufferSize )
			audioMixer.outputReadPosition = 0;

		float data = audioMixer.pOutputBuffer[ audioMixer.outputReadPosition ];
		data += 1.0f;
		data *= range;
		u32 intdata = (u32)data;
		pBuffer[ sample++ ] = intdata;   // Left
		pBuffer[ sample++ ] = intdata;   // Right
	}

	// The entire buffer was filled, yay!
	return nChunkSize;
}
