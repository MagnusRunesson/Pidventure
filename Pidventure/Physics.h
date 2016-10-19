//
//  Physics.h
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-13.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#ifndef Physics_h
#define Physics_h

class CScene;

void physInit( CScene* _pScene );
bool physIsGround( int _x, int _y );
bool physIsWall( int _x, int _y );
bool physTakeSample( int _x, int _y );
void physSetActiveScene( CScene* _pScene );

#endif /* Physics_h */
