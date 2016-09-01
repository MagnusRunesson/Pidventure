//
//  SpriteRenderer.cpp
//  TinyArcade
//
//  Created by Magnus Runesson on 2015-12-26.
//  Copyright © 2015 Magnus Runesson. All rights reserved.
//

#include <stdio.h>
#include "Engine/Graphics/SpriteRenderer.h"
#include "Engine/Graphics/Screen.h"
#include "Engine/Graphics/Image.h"
#include "Engine/Core/BitHelpers.h"
#include "Engine/Core/Debug.h"

SpriteRenderer spriteRenderer;

SpriteRenderer::SpriteRenderer()
{
	Reboot();
}

void SpriteRenderer::Reboot()
{
	int i = 0;
	for( i=0; i<MAX_SPRITES; i++ )
		m_sprite[ i ].Reboot();

	for( i=0; i<MAX_SPRITES+1; i++ )
		m_sortedSprites[ i ] = NULL;
}

Sprite* SpriteRenderer::AllocateSprite( const Image* _image )
{
	//printf("Allocating sprite from image \"%s\"\n", _image->DEBUG_name );
	
	int i = 0;
	for( i=0; i<MAX_SPRITES; i++ )
	{
		Sprite* sprite = &m_sprite[ i ];
		if( sprite->image == NULL )
		{
			sprite->image = _image;
			SetBit( sprite->flags, SPRITE_FLAG_ENABLED );
			sprite->rendererIndex = i;
			//printf("using index %i\n", i );
			
			AddSpriteToSortList( sprite );
			SortAllSprites();
			return sprite;
		}
	}
	
	debugLog("Out of sprites, gangstah!\n");

	return NULL;
}

void SpriteRenderer::AddSpriteToSortList( Sprite* _pSprite )
{
	int i;
	for( i=0; i<MAX_SPRITES; i++ )
	{
		if( m_sortedSprites[ i ] == NULL )
		{
			m_sortedSprites[ i ] = _pSprite;
			return;
		}
	}
}

void SpriteRenderer::FreeSprite( Sprite* _spriteInstance )
{
	//printf("freeing sprite from image %s\n", _spriteInstance->image->DEBUG_name );
	_spriteInstance->image = NULL;
	_spriteInstance->owner = NULL;
	_spriteInstance->rendererIndex = INVALID_SPRITE_RENDERER_INDEX;
	ClrBit( _spriteInstance->flags, SPRITE_FLAG_ENABLED );
}

Sprite* SpriteRenderer::GetSprite( int _spriteRendererIndex )
{
	return &m_sprite[ _spriteRendererIndex ];
}

void SpriteRenderer::SortAllSprites()
{
	SortSprites( m_sortedSprites );
}

void SpriteRenderer::SortSprites( Sprite** _apSprites )
{
	// Bubble sort, woop!
	bool sortAgain = false;

	do
	{
		sortAgain = false;
		int i;
		for( i=MAX_SPRITES-2; i>=0; i-- )
		{
			Sprite* pSprA = _apSprites[ i+0 ];
			Sprite* pSprB = _apSprites[ i+1 ];
			if((pSprA != NULL) && (pSprB != NULL))
			{
				if( pSprA->sort > pSprB->sort )
				{
					// Sprites in the wrong order. Sort!
					_apSprites[ i+0 ] = pSprB;
					_apSprites[ i+1 ] = pSprA;
					sortAgain = true;
				}
			}
		}
	} while( sortAgain );
}

extern const char* stringFromBool( bool );

void SpriteRenderer::debugPrintStats()
{
	debugLog( "----------[ SpriteRenderer ]----------\n" );
	int numUsed = 0;
	int i;
	for( i=0; i<MAX_SPRITES; i++ )
	{
		Sprite* sprite = &m_sprite[ i ];
		debugLog("Sprite object %-3i -", i );
		debugLog(" Used: %-3s", stringFromBool( sprite->image != NULL ));
		if( sprite->image != NULL )
		{
			numUsed++;
			debugLog(" - Image: %s", sprite->image->DEBUG_name );
		}
		debugLog("\n");
	}

	debugLog("Sorted sprites:\n");
	for( i=0; i<MAX_SPRITES; i++ )
	{
		Sprite* sprite = m_sortedSprites[ i ];
		if( sprite == NULL )
			break;
		debugLog("Sprite instance=%i sort=%i\n", sprite->rendererIndex, sprite->sort );
	}
	
	debugLog("Num sprites used: %i / %i\n", numUsed, MAX_SPRITES );
}
