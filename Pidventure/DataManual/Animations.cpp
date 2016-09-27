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

/******************************************************************************************************************************************************************
 *
 * Props
 *
 ******************************************************************************************************************************************************************/
 
const int DATA_FLOWER_FRAME_DURATION = 8;
AnimationFrameDefinition animation_prop_flower_red_frames[] = {
	{
		"sprite_prop_flower_red_af00",
		NULL,
		DATA_FLOWER_FRAME_DURATION,
		0,
		1.0f, 4.0f,
	},
	{
		"sprite_prop_flower_red_af01",
		NULL,
		DATA_FLOWER_FRAME_DURATION,
		0,
		2.0f, 4.0f,
	},
	{
		"sprite_prop_flower_red_af02",
		NULL,
		DATA_FLOWER_FRAME_DURATION,
		0,
		2.0f, 3.0f,
	},
	{
		"sprite_prop_flower_red_af03",
		NULL,
		DATA_FLOWER_FRAME_DURATION,
		0,
		1.0f, 3.0f,
	},
};

DEFINE_ANIMATION( prop_flower_red, true );


AnimationFrameDefinition animation_prop_house_highlands_frames[] = {
	{
		"sprite_prop_house_highlands",
		NULL,
		1,
		0,
		1.0f, 4.0f,
	},
};

DEFINE_ANIMATION( prop_house_highlands, true );


AnimationSequenceDefinition* data_animAll[] = {
	&animation_prop_flower_red,
	&animation_prop_house_highlands,
};

int data_numAnimations = sizeof( data_animAll ) / sizeof( AnimationSequenceDefinition* );
