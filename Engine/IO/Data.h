//
//  Data.h
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-22.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#ifndef Data_h
#define Data_h

#include "Pidventure/DataManual/Animations.h"

void dataInit();
void dataExit();
void dataAddAnimationSequenceDefinition( AnimationSequenceDefinition* _pDefinition );
AnimationSequenceDefinition* dataGetAnimationSequenceDefinition( const char* _pszSpriteName );

#endif /* Data_h */
