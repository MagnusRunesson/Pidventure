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
#include "Pidventure/Player/PlayerAvatar.h"
#include "Pidventure/Physics.h"

CPlayerAvatar::CPlayerAvatar( float _worldX, float _worldY )
{
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
	
}

void CPlayerAvatar::MoveHorizontal( float _delta )
{
	int stepUp = 2;
	int stepDown = 1;
	int numSteps = stepUp + stepDown;

	float newY = m_worldY - ((float)stepUp);
	float newX = m_worldX + _delta;
	int inewx = roundf( newX );
	
	int i;
	for( i=0; i<=numSteps; i++ )
	{
		if( physIsCollision( inewx, (int)newY ))
		{
			break;
		}
		
		newY += 1.0f;
	}

	newY -= 1.0f;
	
	m_worldX += _delta;
	m_worldY = newY;
	m_pGameObject->SetWorldPosition( m_worldX, m_worldY );
}
