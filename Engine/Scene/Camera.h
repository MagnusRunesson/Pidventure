//
//  Camera.hpp
//  TinyArcade
//
//  Created by Magnus Runesson on 2015-12-08.
//  Copyright © 2015 Magnus Runesson. All rights reserved.
//

#ifndef Camera_h
#define Camera_h

class Camera
{
public:
	Camera();
	
	void SetWorldPosition( float _x, float _y );
	inline float GetWorldX(){ return m_worldPositionX; }
	inline float GetWorldY(){ return m_worldPositionY; }

	static Camera* main;

private:
	float m_worldPositionX;
	float m_worldPositionY;
};

#endif /* Camera_h */
