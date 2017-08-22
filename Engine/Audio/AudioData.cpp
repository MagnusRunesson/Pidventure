//
//  AudioData.cpp
//  TinyShooter
//
//  Created by Magnus Runesson on 2016-01-23.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include "Engine/Audio/AudioData.h"

#include "Engine/stdc/stdlib.h"
#include "Engine/Util/String.h"
#include "Engine/Graphics/Image.h"
#include "Engine/IO/File.h"
#include "Engine/Core/Debug.h"
#include "Engine/Core/Memory.h"

static char g_pszAudioDataFullName[ 1024 ];

AudioData* audioDataLoad( const char* _pszFileName, bool _looping )
{
	stringCombine( g_pszAudioDataFullName, 1024, _pszFileName, ".raw" );
	void* pData;
	int readBytes;
	if( fileLoad( g_pszAudioDataFullName, &pData, &readBytes ) == false)
	{
		debugLog("Failed to load audio data '%s.raw'\n", _pszFileName);
		return NULL;
	}

	//
	AudioData* pRet = new AudioData();
	pRet->length = readBytes;
	pRet->looping = _looping;
	pRet->samples = (const sint8*)pData;
	
	//
	return pRet;
}

void audioDataUnload( AudioData* _pAudioData )
{
	//
	fileUnload( (void*)_pAudioData->samples );
	
	//
	delete _pAudioData;
}
