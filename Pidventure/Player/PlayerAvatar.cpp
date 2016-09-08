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

CPlayerAvatar::CPlayerAvatar()
{
	m_pImage = imageLoad("herotest");
	m_pGameObject = gameObjectManager.CreateGameObject( m_pImage );
	m_pGameObject->SetWorldPosition( 30.0f, 10.0f );
}

CPlayerAvatar::~CPlayerAvatar()
{
	imageUnload( m_pImage );
	gameObjectManager.FreeGameObject( m_pGameObject );
}
