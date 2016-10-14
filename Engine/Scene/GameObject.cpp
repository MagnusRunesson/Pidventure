//
//  GameObject.cpp
//  TinyArcade
//
//  Created by Magnus Runesson on 2015-12-03.
//  Copyright © 2015 Magnus Runesson. All rights reserved.
//

#include <stdlib.h>
#include "Engine/Scene/GameObject.h"
#include "Engine/Scene/Camera.h"
#include "Engine/Graphics/Image.h"
#include "Engine/Graphics/SpriteRenderer.h"
#include "Engine/Graphics/Animation.h"
#include "Engine/Core/BitHelpers.h"
#include "Engine/Core/Debug.h"

GameObject::GameObject()
{
	Reboot();
}

void GameObject::Reboot()
{
	m_sprite = NULL;
	m_customUpdate = NULL;
	m_customPreRender = NULL;
	m_customPostRender = NULL;
	m_customObject = NULL;
	m_animation.pSequence = NULL;
}

void GameObject::Create( const Image* _image )
{
	//
	m_worldPositionX = 0;
	m_worldPositionY = 0;
	m_flags = 0;
	
	//
	m_sprite = spriteRenderer.AllocateSprite( _image );
	m_sprite->owner = this;
	m_imageHotspotX = 0;
	m_imageHotspotY = 0;
	
	SetEnabled( true );
}

void GameObject::Create( const AnimationSequenceDefinition* _animation )
{
	m_animationSequenceDefinition = _animation;
	m_animation.Create( _animation, this );
	
	// Create a sprite from the first frame and apply other relevant data from that frame
	AnimationFrameDefinition* pAnimFrame = &_animation->Frames[ 0 ];
	Create( pAnimFrame->sourceImage );
	m_imageHotspotX = pAnimFrame->HotspotX;
	m_imageHotspotY = pAnimFrame->HotspotY;
}

void GameObject::Destroy()
{
	SetEnabled( false );
	spriteRenderer.FreeSprite( m_sprite );
	m_sprite = NULL;
}

void GameObject::SetEnabled( bool _enabled )
{
	
	if( _enabled )
	{
		SetBit( m_systemFlags, GO_SYSTEMFLAG_ENABLED );
		m_sprite->SetFlags( SPRITE_FLAG_ENABLED );
	}
	else
	{
		ClrBit( m_systemFlags, GO_SYSTEMFLAG_ENABLED );
		m_sprite->ClrFlags( SPRITE_FLAG_ENABLED );
	}
}

bool GameObject::IsEnabled()
{
	return HasBit( m_systemFlags, GO_SYSTEMFLAG_ENABLED );
}

void GameObject::Update()
{
	if( m_customUpdate != NULL )
		m_customUpdate( m_customObject );
	
	if( m_animation.pSequence != NULL )
		m_animation.Update();
}

void GameObject::Render()
{
	if( m_customPreRender != NULL )
		m_customPreRender( m_customObject );
	
	m_sprite->x = m_worldPositionX - Camera::main->GetWorldX() - m_imageHotspotX;
	m_sprite->y = m_worldPositionY - Camera::main->GetWorldY() - m_imageHotspotY;

	if( m_customPostRender != NULL )
		m_customPostRender( m_customObject );
}

void GameObject::SetHotspot( float _x, float _y )
{
	m_imageHotspotX = _x;
	m_imageHotspotY = _y;
}


void GameObject::SetWorldPosition( float _x, float _y )
{
	m_worldPositionX = _x;
	m_worldPositionY = _y;
}

float GameObject::GetWorldPositionX()
{
	return m_worldPositionX;
}

float GameObject::GetWorldPositionY()
{
	return m_worldPositionY;
}

bool GameObject::IsUsed()
{
	return !((m_sprite==NULL) && (m_animation.pSequence==NULL));
}

Sprite* GameObject::GetSprite()
{
	return m_sprite;
}

void GameObject::SetImage( const Image* _pImage )
{
	m_sprite->image = _pImage;
}

const AnimationSequenceDefinition* GameObject::GetAnimationSequenceDefinition()
{
	return m_animationSequenceDefinition;
}

Animation* GameObject::GetAnimation()
{
	return &m_animation;
}
