//
//  Player.h
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-02.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#ifndef Player_h
#define Player_h

class CPlayerAvatar;
class CPlayerInventory;

class CPlayer
{
public:
	CPlayer();
	~CPlayer();
	
	void Update();
	
	CPlayerAvatar* m_pAvatar;
	CPlayerInventory* m_pInventory;
};

#endif /* Player_h */
