//
//  Player.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-02.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include "Pidventure/Player/Player.h"
#include "Pidventure/Player/PlayerAvatar.h"
#include "Pidventure/Player/PlayerInventory.h"

CPlayer::CPlayer()
{
	m_pAvatar = new CPlayerAvatar();
	m_pInventory = new CPlayerInventory();
}

CPlayer::~CPlayer()
{
	delete m_pAvatar;
	delete m_pInventory;
}

void CPlayer::Update()
{
	
}
