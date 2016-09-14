//
//  Scene.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-12.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include <stdlib.h>
#include "Engine/Scene/GameObjectManager.h"
#include "Engine/Graphics/Image.h"
#include "Engine/Graphics/Sprite.h"
#include "Pidventure/Scene.h"
#include "Pidventure/Physics.h"

const char* SCENE_GRAPHICSFILE_NAMES[] = {
	"sprite_highlands_bg4",
	"sprite_highlands_ground_edge_right",
	"sprite_highlands_ground_edge_left",
	"sprite_highlands_water_double",
	"sprite_prop_signpost",
	"sprite_prop_flower_red",
	"sprite_prop_flower_red",
	"sprite_prop_flower_red",
	"sprite_prop_house_highlands",
	"sprite_prop_bridge",
};

const int SCENE_NUM_GRAPHICFILES						= sizeof(SCENE_GRAPHICSFILE_NAMES) / sizeof(const char*);

const int SCENE_SPRITE_POSITIONS[] = {
	0, 42,
	0, 60,
	61, 48,
	48, 66,
	35, 51,
	32, 56,
	38, 56,
	26, 56,
	2, 42,
	44, 50,
};

const float SCENE_SPRITE_SORT[] = {
	-4.0f,
	-1.0f,
	-1.0f,
	-2.0f,
	-2.0f,
	2.0f,
	-2.0f,
	2.0f,
	-3.0f,
	1.0f,
};

const int SCENE_MAX_SPRITES								= 20;

GameObject* sceneObjects[ SCENE_MAX_SPRITES ];
Image* sceneImage[ SCENE_MAX_SPRITES ];

void sceneLoad( const char* _pszName )
{
	int i;
	
	for( i=0; i<SCENE_MAX_SPRITES; i++ )
		sceneObjects[ i ] = NULL;

	for( i=0; i<SCENE_NUM_GRAPHICFILES; i++ )
	{
		Image* pImage = imageLoad( SCENE_GRAPHICSFILE_NAMES[ i ]);
		GameObject* pGO = gameObjectManager.CreateGameObject( pImage );
		
		sceneImage[ i ] = pImage;
		sceneObjects[ i ] = pGO;
		
		Sprite* pSprite = pGO->GetSprite();
		pGO->SetWorldPosition( SCENE_SPRITE_POSITIONS[ (i*2)+0 ], SCENE_SPRITE_POSITIONS[ (i*2)+1 ]);
		pSprite->SetSort( SCENE_SPRITE_SORT[ i ]);
		
	}
	
	physInit();
}
