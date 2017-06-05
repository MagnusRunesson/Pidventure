//
//  Font.h
//  Razware Mega Collection 1
//
//  Created by Magnus Runesson on 2017-06-05.
//  Copyright © 2017 Magnus Runesson. All rights reserved.
//

#ifndef Font_h
#define Font_h

class Image;

class FontGlyph
{
public:
	unsigned char ID;
	int rectLeft, rectTop;
	int rectWidth, rectHeight;
	int baseline;
};

class Font
{
public:
	int numGlyphs;
	FontGlyph* glyphs;
	Image* pImage;
	FontGlyph* GetGlyph( unsigned char _glyphID ) const;
};

#endif /* Font_h */
