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
      pBuffer[ sample++ ] = m_sawTooth;   // Left
      pBuffer[ sample++ ] = m_sawTooth;   // Right

      m_sawTooth += (1<<8);
      if( m_sawTooth >= (1<<11))
      {
         m_sawTooth -= (1<<12);
      }
   }

   // The entire buffer was filled, yay!
   return nChunkSize;
}
