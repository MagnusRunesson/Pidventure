//
//  Animations.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-22.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include <stdlib.h>
#include "Engine/Graphics/Animation.h"
#include "Pidventure/DataManual/Animations.h"

const int FRAME_DURATION = 8;
AnimationFrameDefinition data_animPropFlowerRedFrames[] = {
	{
		"sprite_prop_flower_red_af00",
		NULL,
		FRAME_DURATION,
		0,
		1.0f, 4.0f,
	},
	{
		"sprite_prop_flower_red_af01",
		NULL,
		FRAME_DURATION,
		0,
		2.0f, 4.0f,
	},
	{
		"sprite_prop_flower_red_af02",
		NULL,
		FRAME_DURATION,
		0,
		2.0f, 3.0f,
	},
	{
		"sprite_prop_flower_red_af03",
		NULL,
		FRAME_DURATION,
		0,
		1.0f, 3.0f,
	},
};

AnimationSequenceDefinition data_animPropFlowerRed = {
	"animPropFlowerRed",
	sizeof(data_animPropFlowerRedFrames) / sizeof(AnimationFrameDefinition),
	0,
	true,
	data_animPropFlowerRedFrames,
};


AnimationSequenceDefinition* data_animAll[] = {
	&data_animPropFlowerRed,
};

int data_numAnimations = sizeof( data_animAll ) / sizeof( AnimationSequenceDefinition* );
