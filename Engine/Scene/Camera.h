//
//  Camera.hpp
//  TinyArcade
//
//  Created by Magnus Runesson on 2015-12-08.
//  Copyright © 2015 Magnus Runesson. All rights reserved.
//

#ifndef Camera_h
#define Camera_h

#include "Engine/Scene/IWorldPosition.h"

class Camera
{
public:
	Camera();
	
	virtual void SetWorldPosition( float _x, float _y );
	virtual void GetWorldPosition( float* _pOutX, float* _pOutY );
	virtual float GetWorldPositionX(){ return m_worldPositionX; }
	virtual float GetWorldPositionY(){ return m_worldPositionY; }

	static Camera* main;

private:
	float m_worldPositionX;
	float m_worldPositionY;
};

#endif /* Camera_h */
