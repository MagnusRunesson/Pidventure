//
//  PlayerAvatar.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-02.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include "Engine/Graphics/Image.h"
#include "Engine/Scene/GameObjectManager.h"
#include "Pidventure/Player/PlayerAvatar.h"

CPlayerAvatar::CPlayerAvatar( float _worldX, float _worldY )
{
	m_pImage = imageLoad("sprite_herotest");
	m_pGameObject = gameObjectManager.CreateGameObject( m_pImage );
	m_pGameObject->SetWorldPosition( _worldX, _worldY );
}

CPlayerAvatar::~CPlayerAvatar()
{
	imageUnload( m_pImage );
	gameObjectManager.FreeGameObject( m_pGameObject );
}
