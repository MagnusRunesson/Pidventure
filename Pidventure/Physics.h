//
//  Physics.h
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-13.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#ifndef Physics_h
#define Physics_h

void physInit();
bool physIsGround(int _x, int _y);
bool physIsWall(int _x, int _y);

#endif /* Physics_h */
