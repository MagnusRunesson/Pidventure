//
//  ImageAnimation.h
//  TinyShooter
//
//  Created by Magnus Runesson on 20/01/16.
//  Copyright (c) 2016 Magnus Runesson. All rights reserved.
//

#ifndef __TinyShooter__ImageAnimation__
#define __TinyShooter__ImageAnimation__

#include "Engine/Core/Types.h"

class GameObject;
class Image;

class AnimationFrameDefinition
{
public:
	char pszFileName[ 128 ];
	const Image* sourceImage;
	int Duration;
	uint8 Flags;
	float HotspotX;
	float HotspotY;
};

class AnimationSequenceDefinition
{
public:
	const char* name;
	int NumFrames;
	int NumLoops;
	bool Loop;
	AnimationFrameDefinition* Frames;
	
	void LoadImages();
};

class Animation
{
public:
	GameObject* pTarget;
	const AnimationSequenceDefinition* pSequence;
	bool IsPlaying;
	int FrameIndex;
	int FrameTime;
	int LoopCount;
	void (*pfnDoneCallback)();

	void Create( const AnimationSequenceDefinition* _pSequence, GameObject* _pTarget );
	void SetSequence( const AnimationSequenceDefinition* _pSequence );
	void SetDoneCallback( void(*_pfnDoneCallback)());
	void SetLoopCount( int _numLoops );
	void Update();
	void Reset();
	void Play();
	void Stop();
	void RefreshFrame();
};

#define DEFINE_ANIMATION( _name, _looping ) \
AnimationSequenceDefinition animation_##_name = \
{ \
	#_name, \
	sizeof( animation_##_name##_frames ) / sizeof( AnimationFrameDefinition ), \
	-1, \
	_looping, \
	animation_##_name##_frames, \
}

#define DEFINE_ANIMATION_LOOPFRAMES( _name, _looping, _numLoopFrames ) \
const AnimationSequenceDefinition animation_##_name = \
{ \
	#_name, \
	sizeof( animation_##_name##_frames ) / sizeof( AnimationFrameDefinition ), \
	_numLoopFrames, \
	_looping, \
	animation_##_name##_frames \
}

#endif /* defined(__TinyShooter__ImageAnimation__) */
