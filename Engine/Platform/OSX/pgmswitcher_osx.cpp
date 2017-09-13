//
//  pgmswitcher_osx.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2017-10-13.
//  Copyright © 2017 Magnus Runesson. All rights reserved.
//

#include "Engine/Core/Debug.h"

void pgmSwitcherReset()
{
}

void pgmSwitcherSwitch( const char* _pszNewProgramIdentifier )
{
	debugLog("Program switcher - Switch to game with identifier '%s'\n", _pszNewProgramIdentifier );
}
