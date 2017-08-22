//
//  Door.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2016-10-13.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include "Engine/stdc/stdlib.h"
#include "Engine/Scene/GameObjectManager.h"
#include "Engine/Scene/GameObject.h"
#include "Engine/Graphics/Image.h"
#include "Engine/Graphics/Animation.h"
#include "Engine/Graphics/Sprite.h"
#include "Engine/Core/Debug.h"
#include "Engine/IO/Data.h"
#include "Pidventure/Gameplay/Door.h"
#include "Pidventure/DataManual/Animations.h"

CDoorManager doorManager;

/******************************************************************************************************************************************************************************
 *
 * Door manager
 *
 *****************************************************************************************************************************************************************************/

void CDoorManager::Init()
{
	/*
	m_pImageDoorExteriorFrame = imageLoad( "sprite_highlands_doorframe_exterior");
	m_pImageDoorExteriorDoor = imageLoad( "sprite_highlands_door_exterior");
	m_pImageDoorInteriorFrame = imageLoad( "sprite_highlands_door_interior");
	 */

	int i;
	for( i=0; i<DM_MAX_DOORS; i++ )
		m_aDoors[ i ].Init();
	Reset();
}

void CDoorManager::Reset()
{
	m_nextFreeDoor = 0;
	
	int i;
	for( i=0; i<DM_MAX_DOORS; i++ )
		m_aDoors[ i ].Reset();
}

CDoor* CDoorManager::CreateDoor( float _worldX, float _worldY )
{
	if( m_nextFreeDoor >= DM_MAX_DOORS )
	{
		debugLog( "ERROR - DOOR MANAGER: Out of doors\n");
	}
	
	CDoor* pDoor = &m_aDoors[ m_nextFreeDoor ];
	pDoor->m_pGameObject->SetWorldPosition( _worldX, _worldY );
	pDoor->m_pGameObject->SetEnabled( true );
	m_nextFreeDoor++;
	
	return pDoor;
}

CDoor* CDoorManager::GetDoorAt( float _worldX, float _worldY )
{
	const float DOOR_HALF_WIDTH = 6.0f;
	
	int i;
	CDoor* pDoor = &m_aDoors[ 0 ];
	for( i=0; i<m_nextFreeDoor; i++ )
	{
		GameObject* pDoorGO = pDoor->m_pGameObject;
		float doorX = pDoorGO->GetWorldPositionX();
		float doorY = pDoorGO->GetWorldPositionY();

		if((_worldX >= doorX-DOOR_HALF_WIDTH) && (_worldX <= doorX+DOOR_HALF_WIDTH))
			if((_worldY >= doorY-1.0f) && (_worldY <= doorY+1.0f))
				return pDoor;
		
		pDoor++;	// Check next door
	}
	
	return NULL;
}



/******************************************************************************************************************************************************************************
 *
 * Door
 *
 *****************************************************************************************************************************************************************************/
void CDoor::Init()
{
	m_pGameObject = gameObjectManager.CreateGameObject( &animation_highlands_door_closed );
	m_pGameObject->GetSprite()->SetSort( -1.0f );
}

void CDoor::Reset()
{
	m_pGameObject->SetEnabled( false );

	m_pSceneInside = NULL;
	m_pSceneOutside = NULL;
	
	m_isOpen = false;
}

void CDoor::Open()
{
	Animation* pAnim = m_pGameObject->GetAnimation();
	pAnim->SetSequence( &animation_highlands_door_open );
	pAnim->Play();
	m_isOpen = true;
}

void CDoor::Close()
{
	Animation* pAnim = m_pGameObject->GetAnimation();
	pAnim->SetSequence( &animation_highlands_door_close );
	pAnim->Play();
	m_isOpen = false;
}

bool CDoor::IsOpen()
{
	return m_isOpen;
}
