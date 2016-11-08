//
//  SpriteRenderer.hpp
//  TinyArcade
//
//  Created by Magnus Runesson on 2015-12-26.
//  Copyright © 2015 Magnus Runesson. All rights reserved.
//

#ifndef SpriteRenderer_hpp
#define SpriteRenderer_hpp

#include "Engine/Core/Types.h"
#include "Engine/Graphics/Sprite.h"

class Image;

#define MAX_SPRITES (1024)
#define INVALID_SPRITE_RENDERER_INDEX (-1)

class SpriteRenderer
{
public:
	Sprite* m_scanlineSprites[ MAX_SPRITES+1 ];		// This list is NULL terminated, so we need to have room for a trailing NULL in case all sprites should be in this list
	Sprite* m_potentialSprites[ MAX_SPRITES+1 ];	// And so is this
	Sprite* m_sortedSprites[ MAX_SPRITES+1 ];		// This one too! NULL terminated arrays are the BEST!
	
	Sprite m_sprite[ MAX_SPRITES ];
	Sprite* m_collisionSprites[ 8 ];

	SpriteRenderer();
	void Reboot();
	void debugPrintStats();

	Sprite* AllocateSprite( const Image* _image );
	void FreeSprite( Sprite* _spriteInstance );
	Sprite* GetSprite( int _spriteRendererIndex );

	//
	void AddSpriteToSortList( Sprite* _pSprite );
	void RemoveSpriteFromSortList( Sprite* _pSprite );
	void SortAllSprites();
	void SortSprites( Sprite** _apSprites );
	
	//
	void Render();

	//
	uint8 m_currentScanline;

	void SortScanlineSprites();
	void FrameStart();
	void NextScanline( bool _debugPrint );
	void RenderScanline( float* _targetBuffer, uint8* _collisionBits, uint8* _collisionIndices );
};

extern SpriteRenderer spriteRenderer;

#endif /* SpriteRenderer_hpp */
