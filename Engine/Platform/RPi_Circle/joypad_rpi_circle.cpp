//
//  joypad_osx.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2017-08-24.
//  Copyright © 2017 Magnus Runesson. All rights reserved.
//

extern void circlePadHardwareInit();
extern void circlePadHardwareUpdate();

void padHardwareInit()
{
	circlePadHardwareInit();
}

void padHardwareUpdate()
{
	circlePadHardwareUpdate();
}
