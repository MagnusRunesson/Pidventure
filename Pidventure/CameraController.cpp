//
//  CameraController.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-14.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include "Engine/Scene/Camera.h"
#include "Engine/Graphics/Screen.h"
#include "Pidventure/CameraController.h"
#include "Pidventure/Player/PlayerAvatar.h"

Camera camera;
CPlayerAvatar* g_pPlayerAvatar;

void cameraInit( CPlayerAvatar* _pPlayerAvatar )
{
	Camera::main = &camera;
	
	g_pPlayerAvatar = _pPlayerAvatar;
}

void cameraUpdate()
{
	const float CAMERA_BOUNDS = 30.0f;
	int camPlayerDiff = g_pPlayerAvatar->m_worldX - camera.GetWorldX();
	if( camPlayerDiff < CAMERA_BOUNDS )
		camera.SetWorldPosition(camera.GetWorldX()-1.0f, camera.GetWorldY());
	if( camPlayerDiff > SCREEN_WIDTH - CAMERA_BOUNDS )
		camera.SetWorldPosition(camera.GetWorldX()+1.0f, camera.GetWorldY());
}

float cameraWorldX()
{
	return camera.GetWorldX();
}
