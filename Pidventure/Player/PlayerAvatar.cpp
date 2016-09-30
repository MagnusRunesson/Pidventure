//
//  PlayerAvatar.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-02.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include <math.h>
#include "Engine/Graphics/Image.h"
#include "Engine/Scene/GameObjectManager.h"
#include "Engine/IO/Joypad.h"
#include "Engine/Core/Timer.h"
#include "Engine/Core/Debug.h"
#include "Pidventure/Player/PlayerAvatar.h"
#include "Pidventure/Physics.h"

CPlayerAvatar::CPlayerAvatar( float _worldX, float _worldY )
{
	m_movementSpeed = 15.0f;

	m_worldX = _worldX;
	m_worldY = _worldY;
	
	m_pImage = imageLoad("sprite_herotest");
	m_pGameObject = gameObjectManager.CreateGameObject( m_pImage );
	m_pGameObject->SetHotspot( 2.0f, 14.0f );
	m_pGameObject->SetWorldPosition( _worldX, _worldY );
}

CPlayerAvatar::~CPlayerAvatar()
{
	imageUnload( m_pImage );
	gameObjectManager.FreeGameObject( m_pGameObject );
}

void CPlayerAvatar::SetWorldPosition( float _worldX, float _worldY )
{
	m_worldX = _worldX;
	m_worldY = _worldY;
	
	m_pGameObject->SetWorldPosition( m_worldX, m_worldY );
}

void CPlayerAvatar::Update()
{
	float dt = timeDelta();

	if( m_jumpTimer <= 0.0f )
	{
		if( padGetPressed() & PAD_KEYMASK_SECONDARY )
		{
			Jump();
		}
	}
	else
	{
		m_jumpTimer -= dt;
		if( m_jumpTimer < 0.0f )
			m_jumpTimer = 0.0f;
	}
	
	MoveHorizontal( padGetX() * m_movementSpeed * dt );
	
	if( m_jumpTimer > 0.0f )
	{
		float apa = m_jumpTimer*2.0f;
		m_worldY -= (apa*apa);
	}
	else
	{
		MoveVertical();
	}
	
	RefreshGameObject();
}

void CPlayerAvatar::Jump()
{
	if(m_jumpTimer <= 0.0f)
		m_jumpTimer = 0.5f;
}

void CPlayerAvatar::MoveHorizontal( float _delta )
{
	float newX = m_worldX + _delta;
	int inewx = (int)newX;
	
	if( physIsWall( inewx, ((int)m_worldY)-2 ) == false )
		m_worldX = newX;
}

void CPlayerAvatar::MoveVertical()
{
	int stepUp = 2;
	int stepDown = 1;
	int numSteps = stepUp + stepDown;
	
	float newY = m_worldY - ((float)stepUp);
	int x = (int)m_worldX;
	
	int i;
	for( i=0; i<=numSteps; i++ )
	{
		if( physIsGround( x, (int)newY ))
		{
			break;
		}
		
		newY += 1.0f;
	}
	
	newY -= 1.0f;
	
	m_worldY = (int)newY;
}

void CPlayerAvatar::RefreshGameObject()
{
	m_pGameObject->SetWorldPosition( m_worldX, m_worldY );
}
