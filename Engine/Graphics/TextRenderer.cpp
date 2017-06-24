//
//  TextRenderer.cpp
//  Razware Mega Collection 1
//
//  Created by Magnus Runesson on 2017-06-05.
//  Copyright © 2017 Magnus Runesson. All rights reserved.
//

#include <stdlib.h>
#include <string.h>
#include "Engine/Core/Debug.h"
#include "Engine/Graphics/TextRenderer.h"
#include "Engine/Graphics/Font.h"
#include "Engine/Graphics/Screen.h"
#include "Engine/Graphics/Image.h"

TextRenderer textRenderer;

Text::Text()
{
	m_pFont = NULL;
	x = y = 0;
	r = g = b = a = 1.0f;
	sort = 0.0f;
	flags = TEXT_FLAG_ENABLED;
}

inline void blendPixel( int _x, int _y, float _r, float _g, float _b, float _a )
{
	if( _x<0 ) return;
	if( _x>=SCREEN_WIDTH ) return;
	if( _y<0 ) return;
	if( _y>=SCREEN_HEIGHT ) return;
	
	int wrofs = (_y * SCREEN_WIDTH + _x) * 4;
	
	float dstA = 1.0f - _a;
	float dstR = screenBuffer[ wrofs+0 ];
	float dstG = screenBuffer[ wrofs+1 ];
	float dstB = screenBuffer[ wrofs+2 ];
	screenBuffer[ wrofs+0 ] = _r*_a + dstR*dstA;
	screenBuffer[ wrofs+1 ] = _g*_a + dstG*dstA;
	screenBuffer[ wrofs+2 ] = _b*_a + dstB*dstA;
}

void Text::Render()
{
	int carriageX = (int)x;
	int baselineY = (int)y;
	
	int nChars = (int)strlen( m_pszText );
	int i;
	for( i=0; i<nChars; i++ )
	{
		unsigned char glyphID = m_pszText[ i ];
		
		Image* specialImage = NULL;
		if( glyphID == '&' )
		{
			// Special case, yo! Reference to joystick or other special images
			if( i+1 == nChars )
			{
				// This was the last character of the string, treat it as a &
			} else
			{
				i++;
				unsigned char specialChar = m_pszText[ i ];
				if( specialChar == '&' )
				{
					// This is "&&", treat it as a &
				} else {
					specialImage = textRenderer.GetImage( specialChar );
				}
			}
		}
		
		if( specialImage != NULL )
		{
			// Render special image
			int glyphW = specialImage->w;
			int glyphH = specialImage->h;
			
			int glyphX, glyphY;
			for( glyphY=0; glyphY<glyphH; glyphY++ )
			{
				for( glyphX=0; glyphX<glyphW; glyphX++ )
				{
					int rdx = glyphX;
					int rdy = glyphY;
					int rdofs = (rdy*glyphW) + rdx;
					rdofs *= 4;
					
					float srcR = specialImage->pixels[ rdofs+0 ];
					float srcG = specialImage->pixels[ rdofs+1 ];
					float srcB = specialImage->pixels[ rdofs+2 ];
					float srcA = specialImage->pixels[ rdofs+3 ];
					srcA *= a;

					int screenX = carriageX + glyphX;
					int screenY = baselineY - glyphH + glyphY;

					blendPixel( screenX, screenY, srcR, srcG, srcB, srcA );
					
					if( flags & TEXT_FLAG_DROPSHADOW )
					{
						//blendPixel( screenX-1.0f, screenY, 0.0f, 0.0f, 0.0f, srcA );
						//blendPixel( screenX+1.0f, screenY, 0.0f, 0.0f, 0.0f, srcA );
						//blendPixel( screenX, screenY-1.0f, 0.0f, 0.0f, 0.0f, srcA );
						//blendPixel( screenX, screenY+1.0f, 0.0f, 0.0f, 0.0f, srcA );
						blendPixel( screenX+1.0f, screenY+1.0f, 0.0f, 0.0f, 0.0f, srcA );
					}
				}
			}
			
			carriageX += glyphW + 1;
		} else
		{
			//debugLog( "Render glyph: '%c'\n", glyphID );
			FontGlyph* pGlyphData = m_pFont->GetGlyph( glyphID );
			int glyphW = pGlyphData->rectWidth;
			int glyphH = pGlyphData->rectHeight;
			
			//debugLog("Glyph data=0x%08x - width=%i, height=%i\n", pGlyphData, glyphW, glyphH );
			
			int glyphX, glyphY;
			for( glyphY=0; glyphY<glyphH; glyphY++ )
			{
				for( glyphX=0; glyphX<glyphW; glyphX++ )
				{
					int screenX = carriageX + glyphX;
					int screenY = baselineY - pGlyphData->baseline + glyphY;
					
					int rdx = pGlyphData->rectLeft + glyphX;
					int rdy = pGlyphData->rectTop + glyphY;
					int rdofs = (rdy*m_pFont->pImage->w) + rdx;
					rdofs *= 4;
					
					float srcR = r;
					float srcG = g;
					float srcB = b;
					float srcA = m_pFont->pImage->pixels[ rdofs+3 ];
					srcA *= a;
					
					blendPixel( screenX, screenY, srcR, srcG, srcB, srcA );

					if( flags & TEXT_FLAG_DROPSHADOW )
					{
						//blendPixel( screenX-1.0f, screenY, 0.0f, 0.0f, 0.0f, srcA );
						//blendPixel( screenX+1.0f, screenY, 0.0f, 0.0f, 0.0f, srcA );
						//blendPixel( screenX, screenY-1.0f, 0.0f, 0.0f, 0.0f, srcA );
						//blendPixel( screenX, screenY+1.0f, 0.0f, 0.0f, 0.0f, srcA );
						blendPixel( screenX+1.0f, screenY+1.0f, 0.0f, 0.0f, 0.0f, srcA );
					}
				}
			}
			
			carriageX += glyphW + 1;
		}
	}
}

TextRenderer::TextRenderer()
{
	int i;
	for( i=0; i<NUM_SPECIAL_IMAGES; i++ )
		m_specialImages[ i ] = NULL;
}

void TextRenderer::SetImage( unsigned char _ID, Image *_pImage )
{
	m_specialImages[ _ID ] = _pImage;
}

Image* TextRenderer::GetImage( unsigned char _ID )
{
	return m_specialImages[ _ID ];
}

Text* TextRenderer::GetFreeText()
{
	int i;
	for( i=0; i<MAX_TEXTS; i++ )
	{
		if( m_text[ i ].m_pFont == NULL )
			return &m_text[ i ];
	}
	
	return NULL;
}

Text* TextRenderer::AllocatedText( const Font *_pFont, const char *_pszText )
{
	Text* pRet = GetFreeText();
	if( pRet == NULL )
	{
		debugLog( "ERROR: Out of free text objects for TextRenderer\n" );
	}
	
	pRet->m_pFont = _pFont;
	pRet->m_pszText = new char[ strlen( _pszText )];
	strcpy( pRet->m_pszText, _pszText );
	
	return pRet;
}

void TextRenderer::FreeText( Text* _pText )
{
	delete[] _pText->m_pszText;
	_pText->m_pFont = NULL;
}

void TextRenderer::Render()
{
	int i;
	for( i=0; i<MAX_TEXTS; i++ )
	{
		if( m_text[ i ].m_pFont != NULL )
			m_text[ i ].Render();
	}
}
