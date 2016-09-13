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
	m_pAvatar->SetWorldPosition( _x, _y );
}

void CPlayer::Update()
{
	m_pAvatar->Update();
}
