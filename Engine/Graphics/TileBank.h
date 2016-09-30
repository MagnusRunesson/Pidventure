//
//  TileBank.hpp
//  TinyArcade
//
//  Created by Magnus Runesson on 2015-12-27.
//  Copyright © 2015 Magnus Runesson. All rights reserved.
//

#ifndef TileBank_hpp
#define TileBank_hpp

#include "Engine/Core/Types.h"

class CTileBank
{
public:
	uint16 NumTiles;
	uint8 TileWidth;
	uint8 TileHeight;
	float Pixels[];
};

CTileBank* tilebankLoad(const char* _pszFileName);
void tilebankUnload( CTileBank* _pszImage );

#endif /* TileBank_hpp */
