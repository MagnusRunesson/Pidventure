#ifndef _audio_rpi_circle_h
#define _audio_rpi_circle_h

#include <circle/pwmsoundbasedevice.h>
#include <circle/interrupt.h>
#include <circle/types.h>
#include <assert.h>
#include "Engine/Core/Types.h"

class CAudioRPiCircle : public CPWMSoundBaseDevice
{
public:
   CAudioRPiCircle( CInterruptSystem *pInterrupt );
   ~CAudioRPiCircle( void );

   virtual unsigned GetChunk( u32 *pBuffer, unsigned nChunkSize );
};

#endif // _audio_rpi_circle_h
