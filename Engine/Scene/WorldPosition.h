//
//  IWorldPosition.h
//  Engine
//
//  Created by Magnus Runesson on 2017-09-18.
//  Copyright © 2017 Magnus Runesson. All rights reserved.
//

#ifndef IWorldPosition_h
#define IWorldPosition_h

class CWorldPosition
{
public:
	CWorldPosition();
	void SetWorldPosition( float _x, float _y );
	void GetWorldPosition( float* _pOutX, float* _pOutY );
	float GetWorldPositionX();
	float GetWorldPositionY();
	void SetLocalPosition( float _x, float _y );
	void GetLocalPosition( float* _pOutX, float* _pOutY );
	float GetLocalPositionX();
	float GetLocalPositionY();
	void SetParent( CWorldPosition* _pParent );
	CWorldPosition* GetParent();
	
private:
	float m_localX;
	float m_localY;
	CWorldPosition* m_pParent;
};

#endif /* IWorldPosition_h */
