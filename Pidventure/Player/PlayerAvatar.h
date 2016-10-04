//
//  PlayerAvatar.h
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-02.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#ifndef PlayerAvatar_h
#define PlayerAvatar_h

class GameObject;
class Image;

class CPlayerAvatar
{
public:
	// Movement constants
	float m_movementSpeed;
	float m_jumpDuration;
	float m_jumpForce;
	float m_fallDuration;
	float m_fallForce;

	//
	float m_worldX;
	float m_worldY;
	float m_jumpTimer;
	float m_fallTimer;
	
	CPlayerAvatar( float _worldX, float _worldY );
	~CPlayerAvatar();

	Image* m_pImage;
	GameObject* m_pGameObject;
	
	void Update();
	void SetWorldPosition( float _worldX, float _worldY );
	void Jump();
	void MoveHorizontal( float _delta );
	void MoveVertical();
	void RefreshGameObject();
};

#endif /* PlayerAvatar_h */
