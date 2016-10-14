//
//  Door.h
//  pidventure
//
//  Created by Magnus Runesson on 2016-10-13.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#ifndef Door_h
#define Door_h

#define DM_MAX_DOORS		(10)

class GameObject;
class Image;

class CDoor
{
public:
	GameObject* m_pGameObject;
	void Init();
	void Reset();
	
	void Open();
	void Close();
};

class CDoorManager
{
	Image* m_pImageDoorExterior;
	Image* m_pImageDoorInterior;

	CDoor m_aDoors[ DM_MAX_DOORS ];
	int m_nextFreeDoor;
	
public:
	void Init();
	void Reset();
	CDoor* CreateDoor( float _worldX, float _worldY );
	CDoor* GetDoorAt( float _worldX, float _worldY );
};

extern CDoorManager doorManager;

#endif /* Door_h */
