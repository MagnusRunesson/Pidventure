//
//  Data.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-22.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include <stdlib.h>
#include <string.h>
#include "Engine/Graphics/Animation.h"
#include "Engine/Core/Debug.h"
#include "Pidventure/DataManual/Data.h"

class AnimationSequenceDefinition;

void dataInit()
{
	int i;
	for( i=0; i<data_numAnimations; i++ )
		data_animAll[ i ]->LoadImages();
}

void dataExit()
{
	int i;
	for( i=0; i<data_numAnimations; i++ )
		data_animAll[ i ]->UnloadImages();
}

AnimationSequenceDefinition* dataGetAnimationSequenceDefinition( const char* _pszSpriteName )
{
	//debugLog("Looking for animation definition %s\n", _pszSpriteName );
	int i;
	for( i=0; i<data_numAnimations; i++ )
	{
		AnimationSequenceDefinition* pAnim = data_animAll[ i ];
		//debugLog("pAnim->name=%s\n", pAnim->name);
		if(!strcmp( pAnim->name, _pszSpriteName ))
		{
			//debugLog("Found, yay!\n");
			return pAnim;
		}
	}
	
	//debugLog("No found:(\n");
	return NULL;
}
