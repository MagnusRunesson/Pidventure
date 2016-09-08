//
//  Player.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-02.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include "Engine/Scene/GameObject.h"
#include "Engine/IO/Joypad.h"
#include "Engine/Core/Timer.h"
#include "Pidventure/Player/Player.h"
#include "Pidventure/Player/PlayerAvatar.h"
#include "Pidventure/Player/PlayerInventory.h"

CPlayer::CPlayer()
{
	m_worldX = 40.0f;
	m_worldY = 20.0f;
	m_movementSpeed = 15.0f;
	
	m_pAvatar = new CPlayerAvatar( m_worldX, m_worldY );
	m_pInventory = new CPlayerInventory();
}

CPlayer::~CPlayer()
{
	delete m_pAvatar;
	delete m_pInventory;
}

void CPlayer::SetWorldPosition( float _x, float _y )
{
	m_worldX = _x;
	m_worldY = _y;
	m_pAvatar->m_pGameObject->SetWorldPosition( _x, _y );
}

void CPlayer::Update()
{
	float dt = timeDelta();
	
	m_worldX += padGetX() * m_movementSpeed * dt;
	m_worldY += padGetY() * m_movementSpeed * dt;
	
	m_pAvatar->m_pGameObject->SetWorldPosition( m_worldX, m_worldY );
}
