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
	CPlayerAvatar( float _worldX, float _worldY );
	~CPlayerAvatar();

	Image* m_pImage;
	GameObject* m_pGameObject;
};

#endif /* PlayerAvatar_h */
