//
//  Timer.hpp
//  TinyShooter
//
//  Created by Magnus Runesson on 2016-01-16.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#ifndef Timer_hpp
#define Timer_hpp

#include "Engine/Core/Types.h"

void timerInit();

uint32 nanos();
uint32 micros();
uint32 millis();

#endif /* Timer_hpp */
