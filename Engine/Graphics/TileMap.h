//
//  TileMap.hpp
//  TinyArcade
//
//  Created by Magnus Runesson on 2016-01-03.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#ifndef TileMap_hpp
#define TileMap_hpp

#include "Engine/Core/Types.h"

class CTileMap
{
public:
	uint16 Width;
	uint16 Height;
	uint16 Tiles[];
};

CTileMap* tilemapLoad( const char* _pszFileName );
void tilemapUnload( CTileMap* _pszImage );

#endif /* TileMap_hpp */
