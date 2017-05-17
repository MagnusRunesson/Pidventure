//
//  Joypad.hpp
//  TinyArcade
//
//  Created by Magnus Runesson on 2015-12-03.
//  Copyright © 2015 Magnus Runesson. All rights reserved.
//

#ifndef Joypad_hpp
#define Joypad_hpp

#include "Engine/Core/Types.h"

const int PAD_KEYMASK_ACTION_UL		= (1<<0);
const int PAD_KEYMASK_ACTION_UM		= (1<<1);
const int PAD_KEYMASK_ACTION_UR		= (1<<2);
const int PAD_KEYMASK_ACTION_LL		= (1<<3);
const int PAD_KEYMASK_ACTION_LM		= (1<<4);
const int PAD_KEYMASK_ACTION_LR		= (1<<5);
const int PAD_KEYMASK_PGM_L			= (1<<6);
const int PAD_KEYMASK_PGM_R			= (1<<6);

const int PAD_KEYMASK_PRIMARY		= PAD_KEYMASK_ACTION_LL;
const int PAD_KEYMASK_SECONDARY		= PAD_KEYMASK_ACTION_LM;
const int PAD_KEYMASK_SELECT		= PAD_KEYMASK_PGM_L;
const int PAD_KEYMASK_START			= PAD_KEYMASK_PGM_R;

const int PAD_KEYMASK_DPAD_UP		= (1<<16);
const int PAD_KEYMASK_DPAD_DOWN		= (1<<17);
const int PAD_KEYMASK_DPAD_LEFT		= (1<<18);
const int PAD_KEYMASK_DPAD_RIGHT	= (1<<19);

//
//
//
void padInit();
void padUpdate();
uint8 padGet( float* _x, float* _y );
uint8 padGetKeys();
float padGetX();
float padGetY();
uint8 padGetPressed();
uint8 padGetReleased();

#endif /* Joypad_hpp */
