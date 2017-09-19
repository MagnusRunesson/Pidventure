//
//  Camera.hpp
//  TinyArcade
//
//  Created by Magnus Runesson on 2015-12-08.
//  Copyright © 2015 Magnus Runesson. All rights reserved.
//

#ifndef Camera_h
#define Camera_h

#include "Engine/Scene/WorldPosition.h"

class Camera : public CWorldPosition
{
public:
	Camera();
	
	static Camera* main;
};

#endif /* Camera_h */
