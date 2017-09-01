#include "Engine/Audio/AudioMixer.h"
#include "Engine/Platform/RPi_Circle/audio_rpi_circle.h"

#define SAMPLE_RATE     44100

CAudioRPiCircle::CAudioRPiCircle( CInterruptSystem *pInterrupt )
:  CPWMSoundBaseDevice (pInterrupt, SAMPLE_RATE )
{
   assert ((1 << 12) <= GetRange () && GetRange () < (1 << 13));  // 12 bit range

   m_sawTooth = 0;
}

CAudioRPiCircle::~CAudioRPiCircle( void )
{
}

unsigned CAudioRPiCircle::GetChunk( u32 *pBuffer, unsigned nChunkSize )
{
   uint32 sample = 0;
   while( sample < nChunkSize )
   {
      audioMixer.outputReadPosition++;
      if( audioMixer.outputReadPosition >= audioMixer.outputBufferSize )
         audioMixer.outputReadPosition = 0;

      u32 data = (127 + audioMixer.pOutputBuffer[ audioMixer.outputReadPosition ]) << 3;
      pBuffer[ sample++ ] = data;   // Left
      pBuffer[ sample++ ] = data;   // Right
   }

   // The entire buffer was filled, yay!
   return nChunkSize;
}
