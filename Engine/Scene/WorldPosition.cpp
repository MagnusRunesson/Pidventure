#include "Engine/stdc/stdc.h"
#include "Engine/Scene/WorldPosition.h"

CWorldPosition::CWorldPosition()
{
	m_localX = 0.0f;
	m_localY = 0.0f;
	m_pParent = NULL;
}

void CWorldPosition::SetWorldPosition( float _x, float _y )
{
	float offsetX = 0.0f;
	float offsetY = 0.0f;
	if( m_pParent )
	{
		offsetX = -m_pParent->GetWorldPositionX();
		offsetY = -m_pParent->GetWorldPositionY();
	}
	
	m_localX = _x + offsetX;
	m_localY = _y + offsetY;
}

void CWorldPosition::GetWorldPosition( float* _pOutX, float* _pOutY )
{
	*_pOutX = GetWorldPositionX();
	*_pOutY = GetWorldPositionY();
}

float CWorldPosition::GetWorldPositionX()
{
	float out = m_localX;
	if( m_pParent )
		out += m_pParent->GetWorldPositionX();
	
	return out;
}

float CWorldPosition::GetWorldPositionY()
{
	float out = m_localY;
	if( m_pParent )
		out += m_pParent->GetWorldPositionY();
	
	return out;
}

void CWorldPosition::SetLocalPosition( float _x, float _y )
{
	m_localX = _x;
	m_localY = _y;
}

void CWorldPosition::GetLocalPosition( float* _pOutX, float* _pOutY )
{
	*_pOutX = m_localX;
	*_pOutY = m_localY;
}

float CWorldPosition::GetLocalPositionX()
{
	return m_localX;
}

float CWorldPosition::GetLocalPositionY()
{
	return m_localY;
}

void CWorldPosition::SetParent( CWorldPosition* _pParent )
{
	m_pParent = _pParent;
}

CWorldPosition* CWorldPosition::GetParent()
{
	return m_pParent;
}
