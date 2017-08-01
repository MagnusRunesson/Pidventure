//
//  SpriteRenderer.cpp
//  TinyArcade
//
//  Created by Magnus Runesson on 2015-12-26.
//  Copyright © 2015 Magnus Runesson. All rights reserved.
//

#include <memory.h>
#include <stdio.h>
#include <math.h>
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

void SpriteRenderer::RemoveSpriteFromSortList( Sprite* _pSprite )
{
	int i;
	for( i=0; i<MAX_SPRITES; i++ )
	{
		if( m_sortedSprites[ i ] == _pSprite )
		{
			m_sortedSprites[ i ] = NULL;
			
			int j;
			for( j=i+1; j<MAX_SPRITES; j++ )
			{
				m_sortedSprites[ j-1 ] = m_sortedSprites[ j ];
				if( m_sortedSprites[ j ] == NULL )
					return;
			}
			
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
	RemoveSpriteFromSortList( _spriteInstance );
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

void SpriteRenderer::Render()
{
	/*
	int i;
	for(i=0; i<MAX_SPRITES; i++ )
	{
		Sprite* pSprite = m_sortedSprites[ i ];
		if(pSprite == NULL)
			break;
		
		if( pSprite->flags & SPRITE_FLAG_ENABLED )
		{
			int basex = (int)roundf( pSprite->x );
			int basey = (int)roundf( pSprite->y );
			int x, y;
			const Image* pImage = pSprite->image;
			const float* pReadPixels = pImage->pixels;
			int wry = basey;
			int wrx = basex;
			int rdx = 0;
			int rdw = pImage->w;
			int rdy = 0;
			int rdh = pImage->h;
			
			if( wrx < 0 )
			{
				rdx -= wrx;
				wrx = 0;
			}
			
			if( wrx+rdw >= SCREEN_WIDTH ) rdw = SCREEN_WIDTH-wrx;
			if(rdw <= 0) continue;

			if(rdx+rdw > pImage->w) rdw = pImage->w - rdx;
			if(rdw <= 0) continue;

			if( wry < 0 )
			{
				rdy -= wry;
				wry = 0;
			}
			
			if( wry+rdh >= SCREEN_HEIGHT ) rdh = SCREEN_HEIGHT-wry;
			if(rdh <= 0) continue;
			
			if(rdy+rdh > pImage->h) rdh = pImage->h - rdy;
			if(rdh <= 0) continue;
			
			
			for(y=0; y<rdh; y++)
			{
				for(x=0; x<rdw; x++)
				{
					int readofs = (((rdy+y)*pImage->w) + (rdx+x)) * 4;
					int writeOfs = (((wry+y)*SCREEN_WIDTH) + (wrx+x)) * 4;
					
					if( pSprite->sort < screenBuffer[ writeOfs+3 ])
						continue;
					
					float r = pReadPixels[ readofs+0 ];
					float g = pReadPixels[ readofs+1 ];
					float b = pReadPixels[ readofs+2 ];
					float a = pReadPixels[ readofs+3 ];
					if( a > 0.0f )
					{
						float ia = 1.0f-a;
						screenBuffer[ writeOfs+0 ] = (r*a)+(screenBuffer[ writeOfs+0 ]*ia);
						screenBuffer[ writeOfs+1 ] = (g*a)+(screenBuffer[ writeOfs+1 ]*ia);
						screenBuffer[ writeOfs+2 ] = (b*a)+(screenBuffer[ writeOfs+2 ]*ia);
						screenBuffer[ writeOfs+3 ] = pSprite->sort;
					}
				}
			}
		}
	}
	/*/
	
	uint8 collisionBits[ SCREEN_WIDTH ];
	uint8 collisionIndices[ SCREEN_WIDTH<<3 ];
	
	FrameStart();
	int x, y;
	for( y=0; y<SCREEN_HEIGHT; y++)
	{
		memset( collisionBits, 0, sizeof(collisionBits));
		memset( collisionIndices, 0, sizeof(collisionIndices));
		RenderScanline( &screenBuffer[((y*SCREEN_WIDTH)*4)], collisionBits, collisionIndices );
		NextScanline( false );
		
		for( x=0; x<SCREEN_WIDTH; x++ )
		{
			if((collisionBits[ x ] & 0x06) == 0x06 )
			{
				int wr = ((y*SCREEN_WIDTH)+x) * 4;
				screenBuffer[ wr+0 ] = 1.0f;
				screenBuffer[ wr+1 ] = 1.0f;
				screenBuffer[ wr+2 ] = 1.0f;
			}
		}
	}
	/**/
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
			//debugLog(" - Image: %s", sprite->image->DEBUG_name );
			debugLog( " - X: %.2f, Y: %.2f  - Image W: %i, H: %i", sprite->x, sprite->y, sprite->image->w, sprite->image->h );
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




void SpriteRenderer::SortScanlineSprites()
{
	SortSprites( m_scanlineSprites );
}

void SpriteRenderer::FrameStart()
{
	m_currentScanline = 0;
	
	int i;
	for( i=0; i<MAX_SPRITES+1; i++ )
	{
		m_potentialSprites[ i ] = NULL;
		m_scanlineSprites[ i ] = NULL;
	}
	
	int iPot = 0;
	int iCurr = 0;
	for( i=0; i<MAX_SPRITES; i++ )
	{
		Sprite* sprite = m_sortedSprites[ i ]; //&m_sprite[ i ];
		if( sprite == NULL )
			break;
		
		//printf("Sprite index %i. Image=0x%016llx\n", i, sprite->image );
		if( HasBit( sprite->flags, SPRITE_FLAG_ENABLED ) && (sprite->image != NULL))
		{
			// Refresh sprite bounds and other render related flags
			sprite->PreRender();
			
			//
			if( sprite->boundsTop > 0 )
			{
				m_potentialSprites[ iPot ] = sprite;
				sprite->FrameStart();
				iPot++;
			} else if( sprite->boundsBottom > 0 )
			{
				m_scanlineSprites[ iCurr ] = sprite;
				sprite->FrameStart();
				iCurr++;
			}
		}
	}
	
	/*
	 // NULL terminated lists
	 m_potentialSprites[ iPot ] = NULL;
	 m_scanlineSprites[ iCurr ] = NULL;
	 */
}


void SpriteRenderer::NextScanline( bool _debugPrint )
{
	m_currentScanline++;
	
	
	//
	// First we iterate the sprites from the last scanline to see if we can remove any
	//
	Sprite** renderSpriteList = m_scanlineSprites;
	Sprite* sprite = *renderSpriteList;
	while( sprite != NULL )
	{
		if( sprite->boundsBottom == m_currentScanline )
		{
			// This sprite has been passed and should no longer be considered for rendering. Remove it from list and check the next sprite.
			Sprite** removeList = renderSpriteList;
			while( *removeList != NULL )
			{
				*removeList = *(removeList+1);
				removeList++;
			}
		} else
		{
			sprite->NextScanLine();
			
			// Next sprite in list
			renderSpriteList++;
		}
		
		// Pick the next sprite
		sprite = *renderSpriteList;
	}
	
	//
	// Then we iterate the potential sprites and add them to the list of scanline sprites
	//
	bool sortSprites = false;
	Sprite** potentialSpriteList = m_potentialSprites;
	sprite = *potentialSpriteList;
	while( sprite != NULL )
	{
		if( sprite->boundsTop == m_currentScanline )
		{
			// This sprite should be added to the list of rendered sprites
			*renderSpriteList = sprite;
			renderSpriteList++;
			sortSprites = true;
			
			// Also remove this sprite from the list of potential sprites
			Sprite** removeList = potentialSpriteList;
			while( *removeList != NULL )
			{
				*removeList = *(removeList+1);
				removeList++;
			}
		} else
		{
			// If we added a sprite to the render list we also remove that sprite from the list of
			// potential sprites. And if we've removed a sprite from the list of potential sprites
			// then we should not go to the next element in that list. So that is why we only go to
			// the next element of the list when we have not removed an entry from the list.
			potentialSpriteList++;
		}
		
		// Next sprite in the potential sprite list
		sprite = *potentialSpriteList;
	}
	
	if( sortSprites )
		SortScanlineSprites();
	
	//
	if( _debugPrint )
	{
		debugLog( "\n------------[ Scanline: %i\n", m_currentScanline );
		
		debugLog( "Current sprites: \n" );
		renderSpriteList = m_scanlineSprites;
		sprite = *renderSpriteList;
		while( sprite != NULL )
		{
			debugLog( "  Image x=%i, y=%i, w=%i, h=%i, left=%i, top=%i, right=%i, bottom=%i, data=0x%016llx\n", sprite->x, sprite->y, sprite->image->w, sprite->image->h, sprite->boundsLeft, sprite->boundsTop, sprite->boundsRight, sprite->boundsBottom, sprite->image->pixels );
			renderSpriteList++;
			sprite = *renderSpriteList;
		}
		
		debugLog( "Upcoming sprites: \n" );
		renderSpriteList = m_potentialSprites;
		sprite = *renderSpriteList;
		while( sprite != NULL )
		{
			debugLog( "  Image x=%i, y=%i, w=%i, h=%i, left=%i, top=%i, right=%i, bottom=%i, data=0x%016llx\n", sprite->x, sprite->y, sprite->image->w, sprite->image->h, sprite->boundsLeft, sprite->boundsTop, sprite->boundsRight, sprite->boundsBottom, sprite->image->pixels );
			renderSpriteList++;
			sprite = *renderSpriteList;
		}
	}
}

void SpriteRenderer::RenderScanline( float* _targetBuffer, uint8* _collisionBits, uint8* _collisionIndices )
{
	Sprite** sprites = m_scanlineSprites;
	Sprite* sprite = *sprites;
	
	while( sprite != NULL )
	{
		const Image* image = sprite->image;
		const Image* imageDepth = sprite->imageDepth;
		
		int drawLength = image->w;
		
		bool flipX = sprite->flags & SPRITE_FLAG_FLIP_X;
		bool flipY = sprite->flags & SPRITE_FLAG_FLIP_Y;
		
		int readStart = sprite->readY;
		
		if( flipX )
			readStart += sprite->image->w;
		
		readStart *= 4;
		
		const float* color = &image->pixels[ readStart ];
		const float* depth = NULL;
		
		if( sprite->flags & SPRITE_FLAG_USEDEPTH )
			depth = &imageDepth->pixels[ readStart ];
		
		int drawx = sprite->boundsLeft;
		if( drawx < 0 )
		{
			int skip = -drawx;
			drawx = 0;
			if( flipX )
			{
				color -= skip*4;
				depth -= skip*4;
			}
			else
			{
				color += skip*4;
				depth += skip*4;
			}
		
			drawLength -= skip;
		}
		
		int apa = drawx + drawLength;
		if( apa >= SCREEN_WIDTH )
		{
			drawLength -= (apa - SCREEN_WIDTH);
		}
		
		float* outBuffer = &_targetBuffer[ drawx*4 ];
		uint8* collisionBits = &_collisionBits[ drawx ];
		uint8* collisionIndices = &_collisionIndices[ drawx << 3 ];
		
		while( drawLength > 0 )
		{
			drawLength--;
			
			// Read color
			float r,g,b,a;
			if( flipX )
			{
				color--; a = *color;
				color--; b = *color;
				color--; g = *color;
				color--; r = *color;
			} else
			{
				r = *color; color++;
				g = *color; color++;
				b = *color; color++;
				a = *color; color++;
			}
			
			if( sprite->flags & SPRITE_FLAG_DRAWWHITE )
			{
				r = 1.0f;
				g = 1.0f;
				b = 1.0f;
			}
			
			// Read depth
			float d = 0.0f;

			if( sprite->flags & SPRITE_FLAG_USEDEPTH )
			{
				if( flipX )	depth -= 4;
				d = *depth;
				if( !flipX ) depth+=4;
			}
			
			if( a == 0.0f )
			{
				// Skip
				outBuffer += 4;
				collisionBits++;
				collisionIndices += 8;
			} else if( a == 1.0f )
			{
				// Full overdraw
				float depth = outBuffer[ 3 ];
				if( depth <= sprite->sort+d )
				{
					*outBuffer = r; outBuffer++;
					*outBuffer = g; outBuffer++;
					*outBuffer = b; outBuffer++;
					*outBuffer = sprite->sort+d; outBuffer++;
				} else {
					outBuffer += 4;
				}
				
				
				// Always detect collision, regardless of depth test result
				*collisionBits |= (1<<sprite->collisionIndex);
				collisionIndices[ sprite->collisionIndex ] = sprite->rendererIndex;
				collisionBits++;
				collisionIndices += 8;
				
				/*
				 *_pOutPixel = rgb;
				 *_pOutCollisionMask |= (1<<sprite->collisionIndex);
				 m_collisionSprites[ sprite->collisionIndex ] = sprite;
				 didRender = true;
				 */
			}
			else
			{
				/*
				uint16 srccol = ((rgb&0x00ff)<<8) + ((rgb&0xff00)>>8);
				
				uint32 srcr = (srccol >> COLORSHIFT_16_R) & COLORWIDTH_16_R;
				uint32 srcg = (srccol >> COLORSHIFT_16_G) & COLORWIDTH_16_G;
				uint32 srcb = (srccol >> COLORSHIFT_16_B) & COLORWIDTH_16_B;
				
				uint32 dsta = 255-alpha;
				uint16 dstcol2 = *outBuffer;
				uint16 dstcol = ((dstcol2&0x00ff)<<8) + ((dstcol2&0xff00)>>8);
				
				uint32 dstr = (dstcol >> COLORSHIFT_16_R) & COLORWIDTH_16_R;
				uint32 dstg = (dstcol >> COLORSHIFT_16_G) & COLORWIDTH_16_G;
				uint32 dstb = (dstcol >> COLORSHIFT_16_B) & COLORWIDTH_16_B;
				
				// Multiply
				uint32 outr = (((srcr*alpha)+(dstr*dsta)) >> 8) & COLORWIDTH_16_R;
				uint32 outg = (((srcg*alpha)+(dstg*dsta)) >> 8) & COLORWIDTH_16_G;
				uint32 outb = (((srcb*alpha)+(dstb*dsta)) >> 8) & COLORWIDTH_16_B;
				 */
				
				float dstr = outBuffer[ 0 ];
				float dstg = outBuffer[ 1 ];
				float dstb = outBuffer[ 2 ];
				float dstSort = outBuffer[ 3 ];
				if( dstSort <= sprite->sort+d )
				{
					float dsta = 1.0f-a;

					/*
					 // Additive
					 uint32 outr = ((srcr*srca)+(dstr<<8)) >> 8;
					 uint32 outg = ((srcg*srca)+(dstg<<8)) >> 8;
					 uint32 outb = ((srcb*srca)+(dstb<<8)) >> 8;
					 if( outr > COLORWIDTH_16_R ) outr = COLORWIDTH_16_R;
					 if( outg > COLORWIDTH_16_G ) outg = COLORWIDTH_16_G;
					 if( outb > COLORWIDTH_16_B ) outb = COLORWIDTH_16_B;
					 */
				
					*outBuffer = (r*a) + (dstr*dsta); outBuffer++;
					*outBuffer = (g*a) + (dstg*dsta); outBuffer++;
					*outBuffer = (b*a) + (dstb*dsta); outBuffer++;
					*outBuffer = sprite->sort+d; outBuffer++;

				} else {
					outBuffer += 4;
				}
				
				// Always detect collision, regardless of depth test result
				*collisionBits |= (1<<sprite->collisionIndex);
				collisionIndices[ sprite->collisionIndex ] = sprite->rendererIndex;
				collisionBits++;
				collisionIndices += 8;
			}
		}
		sprites++;
		sprite = *sprites;
	}
}
