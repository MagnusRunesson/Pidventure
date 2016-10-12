//
//  PlayerAvatar.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-02.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include <math.h>
#include "Engine/Graphics/Image.h"
#include "Engine/Graphics/Sprite.h"
#include "Engine/Scene/GameObjectManager.h"
#include "Engine/IO/Joypad.h"
#include "Engine/Core/Timer.h"
#include "Engine/Core/Debug.h"
#include "Pidventure/Player/PlayerAvatar.h"
#include "Pidventure/Physics.h"

CPlayerAvatar::CPlayerAvatar( float _worldX, float _worldY )
{
	debugLog("pa 0\n");
	m_movementSpeed = 30.0f;
	m_jumpDuration = 0.6f;
	m_jumpForce = 1.9f;
	m_fallDuration = m_jumpDuration;
	m_fallForce = m_jumpForce * 1.2f;

	m_worldX = _worldX;
	m_worldY = _worldY;
	m_jumpTimer = 0.0f;
	m_fallTimer = 0.0f;
	m_wasGrounded = true;
	
	debugLog("pa 1\n");
	m_pImage = imageLoad("sprite_herotest");
	debugLog("pa 2\n");
	m_pGameObject = gameObjectManager.CreateGameObject( m_pImage );
	debugLog("pa 3\n");
	m_pGameObject->SetHotspot( 2.0f, 14.0f );
	m_pGameObject->SetWorldPosition( _worldX, _worldY );
	m_pGameObject->GetSprite()->collisionIndex = 2;
	debugLog("pa 4\n");
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
		if( m_wasGrounded && (padGetPressed() & PAD_KEYMASK_SECONDARY))
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
		float apa = m_jumpTimer*m_jumpForce;
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
		m_jumpTimer = m_jumpDuration;
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
	int stepDown = 2;
	int numSteps = stepUp + stepDown;
	
	float newY = m_worldY - ((float)stepUp);
	int x = (int)m_worldX;

	bool foundGround = false;
	int i;
	for( i=0; i<=numSteps; i++ )
	{
		if( physIsGround( x, (int)newY ))
		{
			foundGround = true;
			break;
		}
		
		newY += 1.0f;
	}
	
	if( foundGround )
	{
		newY -= 1.0f;
		m_worldY = (int)newY;
		m_fallTimer = 0.0f;
	} else {
		if( m_fallTimer < m_fallDuration )
		{
			m_fallTimer += timeDelta();
			if( m_fallTimer > m_fallDuration )
				m_fallTimer = m_fallDuration;
		}
		
		float a = m_fallTimer*m_fallForce;
		
		float oldWorld = m_worldY;
		float newWorld = m_worldY + a*a;
		int oldWorldI = (int)oldWorld;
		int newWorldI = (int)newWorld;
		for( i=oldWorldI; i<newWorldI; i++ )
		{
			if( physIsGround( x, i ))
			{
				foundGround = true;
				break;
			}
		}
		
		if( foundGround )
		{
			m_worldY = i;
		} else {
			m_worldY = newWorld;
		}
	}
	
	// Remember for next frame
	m_wasGrounded = foundGround;
}

void CPlayerAvatar::RefreshGameObject()
{
	m_pGameObject->SetWorldPosition( m_worldX, m_worldY );
}
