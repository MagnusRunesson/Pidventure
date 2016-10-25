//
//  CameraController.h
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-14.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#ifndef CameraController_h
#define CameraController_h

class CPlayerAvatar;
class CScene;

void cameraInit( CPlayerAvatar* _pPlayerAvatar );
void cameraSetBounds( CScene* _pScene );
void cameraUpdate();
float cameraWorldX();
float cameraWorldY();

#endif /* CameraController_h */
