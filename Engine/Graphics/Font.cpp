//
//  Font.cpp
//  Razware Mega Collection 1
//
//  Created by Magnus Runesson on 2017-06-05.
//  Copyright © 2017 Magnus Runesson. All rights reserved.
//

#include "Font.h"

FontGlyph* Font::GetGlyph( unsigned char _glyphID ) const
{
	int i;
	for( i=0; i<numGlyphs; i++ )
	{
		if( glyphs[ i ].ID == _glyphID )
			return &glyphs[ i ];
	}
	
	return &glyphs[ 0 ];
}
