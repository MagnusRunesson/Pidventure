//
//  AudioData.h
//  TinyShooter
//
//  Created by Magnus Runesson on 2016-01-23.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#ifndef AudioData_h
#define AudioData_h

#include "Engine/Core/Types.h"

class AudioData
{
public:
	uint32 length;
	bool looping;
	const sint8* samples;
};

AudioData* audioDataLoad( const char* _pszFileName, bool _looping );
void audioDataUnload( AudioData* _pAudioData );

#endif /* AudioData_h */
