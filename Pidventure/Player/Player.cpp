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
#include "Engine/Core/Debug.h"
#include "Engine/Core/Memory.h"
#include "Pidventure/Player/Player.h"
#include "Pidventure/Player/PlayerAvatar.h"
#include "Pidventure/Player/PlayerInventory.h"

CPlayer::CPlayer()
{
	debugLog("Player a\n");
	m_Avatar.SetWorldPosition( m_worldX, m_worldY );
	debugLog("Player c\n");
}

CPlayer::~CPlayer()
{
}

void CPlayer::SetWorldPosition( float _x, float _y )
{
	m_Avatar.SetWorldPosition( _x, _y );
}

void CPlayer::Update()
{
	m_Avatar.Update();
}
