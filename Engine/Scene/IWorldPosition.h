//
//  IWorldPosition.h
//  Engine
//
//  Created by Magnus Runesson on 2017-09-18.
//  Copyright © 2017 Magnus Runesson. All rights reserved.
//

#ifndef IWorldPosition_h
#define IWorldPosition_h

class IWorldPosition
{
public:
	virtual void SetWorldPosition( float _x, float _y ) = 0;
	virtual void GetWorldPosition( float* _pOutX, float* _pOutY ) = 0;
	virtual float GetWorldPositionX() = 0;
	virtual float GetWorldPositionY() = 0;
};

#endif /* IWorldPosition_h */
