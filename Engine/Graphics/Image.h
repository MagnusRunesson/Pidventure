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
	float pixels[];
};

#endif /* Image_hpp */
