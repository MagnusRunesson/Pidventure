//
//  TextRenderer.h
//  Razware Mega Collection 1
//
//  Created by Magnus Runesson on 2017-06-05.
//  Copyright © 2017 Magnus Runesson. All rights reserved.
//

#ifndef TextRenderer_h
#define TextRenderer_h

#include "Engine/Core/Types.h"

class Font;
class Image;
class TextRenderer;

#define MAX_TEXTS (1024)
#define NUM_SPECIAL_IMAGES (128)
#define TEXT_FLAG_ENABLED (1<<0)

class Text
{
	friend class TextRenderer;
	
	const Font* m_pFont;
	char* m_pszText;
	
	void Render();
	void RenderGlyph();
	
public:
	Text();
	
	float x, y;
	float r, g, b, a;
	float sort;
	uint32 flags;
};

class TextRenderer
{
	Text m_text[ MAX_TEXTS ];
	Image* m_specialImages[ NUM_SPECIAL_IMAGES ];

	Text* GetFreeText();
	
public:
	TextRenderer();
	void SetImage( unsigned char _ID, Image* _pImage );
	Image* GetImage( unsigned char _ID );
	Text* AllocatedText( const Font* _pFont, const char* _pszText );
	void FreeText( Text* _pText );
	void Render();
};

extern TextRenderer textRenderer;

#endif /* TextRenderer_h */
