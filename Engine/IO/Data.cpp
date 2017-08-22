//
//  Data.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-22.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include "Engine/stdc/stdlib.h"
#include "Engine/Util/String.h"
#include "Engine/Graphics/Animation.h"
#include "Engine/Core/Debug.h"

#define DATA_MAX_ANIMATIONS			(64)

int dataNumAnimations;

AnimationSequenceDefinition* dataAnimationSequenceDefinitions[ DATA_MAX_ANIMATIONS ];

void dataInit()
{
	dataNumAnimations = 0;
}

void dataExit()
{
	int i;
	for( i=0; i<dataNumAnimations; i++ )
		dataAnimationSequenceDefinitions[ i ]->UnloadImages();
}

void dataAddAnimationSequenceDefinition( AnimationSequenceDefinition* _pDefinition )
{
	dataAnimationSequenceDefinitions[ dataNumAnimations ] = _pDefinition;
	_pDefinition->LoadImages();
	dataNumAnimations++;
}

AnimationSequenceDefinition* dataGetAnimationSequenceDefinition( const char* _pszSpriteName )
{
	//debugLog("Looking for animation definition %s\n", _pszSpriteName );
	int i;
	for( i=0; i<dataNumAnimations; i++ )
	{
		AnimationSequenceDefinition* pAnim = dataAnimationSequenceDefinitions[ i ];
		//debugLog("pAnim->name=%s\n", pAnim->name);
		if( !stringCompare( pAnim->name, _pszSpriteName ))
		{
			//debugLog("Found, yay!\n");
			return pAnim;
		}
	}
	
	//debugLog("No found:(\n");
	return NULL;
}
