//
//  Scene.h
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-12.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#ifndef Scene_h
#define Scene_h

#include "Engine/Graphics/TileRenderer.h"

const int SCENE_MAX_SPRITES								= 2048;

class CTileMap;
class CTileBank;
class GameObject;

class CScene
{
public:
	CTileBank* pTileBank;
	CTileBank* pTileBankCollision;
	CTileMap* pTileMap;
	TileRenderer TileRenderer;
	
	GameObject* sceneObjects[ SCENE_MAX_SPRITES ];
	int m_worldX;
	int m_worldY;
	
	CScene();
	~CScene();

	bool Load( const char* _pszName );

	void SetWorldPosition( int _x, int _y );
	void SetSort( float _sort );
	void SetViewportTopLeft( int _x, int _y );
	void Render();

	int GetWorldTop();
	int GetWorldBottom();
	int GetWorldLeft();
	int GetWorldRight();
};

/*
void sceneLoad( const char* _pszName );
void sceneUpdate();
void sceneUnload();
*/

#endif /* Scene_h */
