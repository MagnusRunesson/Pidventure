//
//  Scene.h
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-12.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#ifndef Scene_h
#define Scene_h

const int SCENE_MAX_SPRITES								= 2048;

class CScene
{
	GameObject* sceneObjects[ SCENE_MAX_SPRITES ];

public:
	CScene();
	bool Load( const char* _pszName );
};

/*
void sceneLoad( const char* _pszName );
void sceneUpdate();
void sceneUnload();
*/

#endif /* Scene_h */
