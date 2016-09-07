//
//  Sprite.hpp
//  TinyArcade
//
//  Created by Magnus Runesson on 2015-12-27.
//  Copyright © 2015 Magnus Runesson. All rights reserved.
//

#ifndef Sprite_hpp
#define Sprite_hpp

#include "Engine/Core/Types.h"

class Image;

//
// Sprite flags
//
#define SPRITE_FLAG_ROTATE_BASE		(0)
#define SPRITE_FLAG_ROTATE_MASK		(3<<SPRITE_FLAG_ROTATE_BASE)
#define SPRITE_FLAG_ROTATE_0		(0<<SPRITE_FLAG_ROTATE_BASE)
#define SPRITE_FLAG_ROTATE_90		(1<<SPRITE_FLAG_ROTATE_BASE)
#define SPRITE_FLAG_ROTATE_180		(2<<SPRITE_FLAG_ROTATE_BASE)
#define SPRITE_FLAG_ROTATE_270		(3<<SPRITE_FLAG_ROTATE_BASE)
#define SPRITE_FLAG_FLIP_X			(1<<3)
#define SPRITE_FLAG_FLIP_Y			(1<<4)
#define SPRITE_FLAG_DRAWWHITE		(1<<5)
#define SPRITE_FLAG_ENABLED			(1<<7)

class GameObject;

class Sprite
{
public:
	float x;
	float y;
	uint32 flags;
	uint8 collisionIndex;
	int rendererIndex;	// A reference back to this sprite via the spriteRenderer (so the game code only have to have an uint8 as a handle to this sprite instead of a 4 byte Sprite pointer)
	float sort;

	sint16 boundsLeft;
	sint16 boundsRight;
	sint16 boundsTop;
	sint16 boundsBottom;
	
	const Image* image;
	GameObject* owner;
	
	Sprite();
	void Reboot();
	
	//
	void SetRotation( int _rotation );
	void SetFlippedX( bool _flipped );
	void SetFlippedY( bool _flipped );
	void SetSort( sint8 _newSort );

	//
	inline void SetFlags( int _flags ) { flags |= _flags; };
	inline void ClrFlags( int _flags ) { flags &= ~_flags; };
};


#endif /* Sprite_hpp */
