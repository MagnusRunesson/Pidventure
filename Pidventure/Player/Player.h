//
//  Player.h
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-02.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#ifndef Player_h
#define Player_h

#include "Pidventure/Player/PlayerAvatar.h"
#include "Pidventure/Player/PlayerInventory.h"

class CPlayer
{
public:
	CPlayer();
	~CPlayer();
	
	void Update();
	void SetWorldPosition( float _x, float _y );
	
	CPlayerAvatar m_Avatar;
	CPlayerInventory m_Inventory;
	
private:
	float m_worldX;
	float m_worldY;
	float m_movementSpeed;
};

#endif /* Player_h */
