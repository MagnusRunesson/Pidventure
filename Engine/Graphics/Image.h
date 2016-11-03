//
//  Image.hpp
//  TinyArcade
//
//  Created by Magnus Runesson on 2015-12-03.
//  Copyright © 2015 Magnus Runesson. All rights reserved.
//

#ifndef Image_hpp
#define Image_hpp

#include "Engine/Core/Types.h"

class Image
{
public:
	uint32 header;
	uint16 w;
	uint16 h;
	uint32 flags;
	uint32 padding;
	float pixels[];
};

Image* imageLoad( const char* _pszFileName );
void imageUnload( Image* _pImage );
void imageUnload( const Image* _pImage );

#endif /* Image_hpp */
