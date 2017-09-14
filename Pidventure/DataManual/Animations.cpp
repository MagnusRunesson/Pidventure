//
//  Animations.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-22.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include "Engine/stdc/stdc.h"
#include "Engine/Graphics/Animation.h"
#include "Engine/IO/Data.h"
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
		16.0f, 17.0f,
	},
};

DEFINE_ANIMATION( prop_house_highlands, true );



/******************************************************************************************************************************************************************
 *
 * Doors
 *
 ******************************************************************************************************************************************************************/

//
// Closed to open
//
const int DATA_DOOR_EXTERIOR_FRAME_DURATION = 8;
AnimationFrameDefinition animation_highlands_door_open_frames[] = {
	{
		"sprite_highlands_door_exterior_af01",
		NULL,
		DATA_DOOR_EXTERIOR_FRAME_DURATION,
		0,
		6.0f, 16.0f,
	},
	{
		"sprite_highlands_door_exterior_af02",
		NULL,
		DATA_DOOR_EXTERIOR_FRAME_DURATION,
		0,
		6.0f, 16.0f,
	},
	{
		"sprite_highlands_door_exterior_af03",
		NULL,
		DATA_DOOR_EXTERIOR_FRAME_DURATION,
		0,
		6.0f, 16.0f,
	},
	{
		"sprite_highlands_door_interior",
		NULL,
		DATA_DOOR_EXTERIOR_FRAME_DURATION,
		0,
		6.0f, 16.0f,
	},
};

DEFINE_ANIMATION( highlands_door_open, false );


//
// Opened to closed
//
AnimationFrameDefinition animation_highlands_door_close_frames[] = {
	{
		"sprite_highlands_door_exterior_af03",
		NULL,
		DATA_DOOR_EXTERIOR_FRAME_DURATION,
		0,
		6.0f, 16.0f,
	},
	{
		"sprite_highlands_door_exterior_af02",
		NULL,
		DATA_DOOR_EXTERIOR_FRAME_DURATION,
		0,
		6.0f, 16.0f,
	},
	{
		"sprite_highlands_door_exterior_af01",
		NULL,
		DATA_DOOR_EXTERIOR_FRAME_DURATION,
		0,
		6.0f, 16.0f,
	},
	{
		"sprite_highlands_door_exterior_af00",
		NULL,
		DATA_DOOR_EXTERIOR_FRAME_DURATION,
		0,
		6.0f, 16.0f,
	},
};

DEFINE_ANIMATION( highlands_door_close, false );


//
// Just closed - initial state of doors
//
AnimationFrameDefinition animation_highlands_door_closed_frames[] = {
	{
		"sprite_highlands_door_exterior_af00",
		NULL,
		DATA_DOOR_EXTERIOR_FRAME_DURATION,
		0,
		6.0f, 16.0f,
	},
};

DEFINE_ANIMATION( highlands_door_closed, false );




/******************************************************************************************************************************************************************
 *
 *
 * List all animations
 *
 *
 ******************************************************************************************************************************************************************/

void animationDataInit()
{
	dataAddAnimationSequenceDefinition( &animation_prop_flower_red );
	dataAddAnimationSequenceDefinition( &animation_prop_house_highlands );
	dataAddAnimationSequenceDefinition( &animation_highlands_door_closed );
	dataAddAnimationSequenceDefinition( &animation_highlands_door_open );
	dataAddAnimationSequenceDefinition( &animation_highlands_door_close );
}
