//
//  CameraController.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-14.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include "Engine/Scene/Camera.h"
#include "Engine/Graphics/Screen.h"
#include "Engine/Graphics/TileMap.h"
#include "Engine/Graphics/TileBank.h"
#include "Engine/Core/Debug.h"
#include "Engine/Scene/Scene.h"
#include "Pidventure/CameraController.h"
#include "Pidventure/Player/PlayerAvatar.h"

Camera camera;

static float g_camX;
static float g_camY;
static float g_camBoundsLeft;
static float g_camBoundsRight;
static float g_camBoundsTop;
static float g_camBoundsBottom;
static CPlayerAvatar* g_pPlayerAvatar;

void cameraInit( CPlayerAvatar* _pPlayerAvatar )
{
	Camera::main = &camera;

	g_pPlayerAvatar = _pPlayerAvatar;
	g_camX = g_pPlayerAvatar->m_worldX-40.0;
	g_camY = g_pPlayerAvatar->m_worldY-50.0f;
	debugLog("Camera x=%.2f, y=%.2f\n", g_camX, g_camY );
	camera.SetWorldPosition( g_camX, g_camY );
}

void cameraSetBounds( CScene* _pScene )
{
	g_camBoundsLeft = _pScene->m_worldX;
	g_camBoundsRight = _pScene->m_worldX + _pScene->pTileMap->Width*_pScene->pTileBank->TileWidth - SCREEN_WIDTH;
	
	g_camBoundsTop = _pScene->m_worldY;
	g_camBoundsBottom = _pScene->m_worldY + _pScene->pTileMap->Height*_pScene->pTileBank->TileHeight - SCREEN_HEIGHT;

	if( g_camX < g_camBoundsLeft ) g_camX = g_camBoundsLeft;
	if( g_camX > g_camBoundsRight ) g_camX = g_camBoundsRight;
	if( g_camY < g_camBoundsTop ) g_camY = g_camBoundsTop;
	if( g_camY > g_camBoundsBottom ) g_camY = g_camBoundsBottom;
	
	debugLog( "New camera bounds, l=%.2f, r=%.2f\n", g_camBoundsLeft, g_camBoundsRight );
}

void cameraUpdate()
{
	const float CAMERA_BOUNDS = 40.0f;
	const float CAMERA_BOUNDS_TELEPORT = 100.0f;
	
	int camPlayerDiff = g_pPlayerAvatar->m_worldX - g_camX;
	if( camPlayerDiff < -CAMERA_BOUNDS_TELEPORT )
	{
		g_camX = g_pPlayerAvatar->m_worldX - CAMERA_BOUNDS;
		if( g_camX < g_camBoundsLeft )
			g_camX = g_camBoundsLeft;
	} else if( camPlayerDiff < CAMERA_BOUNDS )
	{
		g_camX -= 1.0f;
		if( g_camX < g_camBoundsLeft )
			g_camX = g_camBoundsLeft;
	}

	if( camPlayerDiff > CAMERA_BOUNDS_TELEPORT )
	{
		g_camX = g_pPlayerAvatar->m_worldX - CAMERA_BOUNDS;
		if( g_camX > g_camBoundsRight )
			g_camX = g_camBoundsRight;
	} else if( camPlayerDiff > SCREEN_WIDTH - CAMERA_BOUNDS )
	{
		g_camX += 1.0f;
		if( g_camX > g_camBoundsRight )
			g_camX = g_camBoundsRight;
	}
	
	camPlayerDiff = g_pPlayerAvatar->m_worldY - g_camY;
	if( camPlayerDiff < -CAMERA_BOUNDS_TELEPORT )
	{
		g_camY = g_pPlayerAvatar->m_worldY - CAMERA_BOUNDS;
		if( g_camY < g_camBoundsTop )
			g_camY = g_camBoundsTop;
	} else if( camPlayerDiff < CAMERA_BOUNDS )
	{
		g_camY -= 1.0f;
		if( g_camY < g_camBoundsTop )
			g_camY = g_camBoundsTop;
	}
	
	if( camPlayerDiff > CAMERA_BOUNDS_TELEPORT )
	{
		g_camY = g_pPlayerAvatar->m_worldY - CAMERA_BOUNDS;
		if( g_camY > g_camBoundsBottom )
			g_camY = g_camBoundsBottom;
	} else if( camPlayerDiff > SCREEN_WIDTH - CAMERA_BOUNDS )
	{
		g_camY += 1.0f;
		if( g_camY > g_camBoundsBottom )
			g_camY = g_camBoundsBottom;
	}
	
	camera.SetWorldPosition( g_camX, g_camY );
}

float cameraWorldX()
{
	return camera.GetWorldX();
}

float cameraWorldY()
{
	return camera.GetWorldY();
}