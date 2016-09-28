//
//  main.cpp
//  tmx2c
//
//  Created by Magnus Runesson on 2016-01-02.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <sstream>
#include <stdio.h>
#include "tinyxml2.h"

using namespace tinyxml2;
using namespace std;

/************************************************************************************************************************************************************************

 
 Output data format
 
 
 */

#define OSO_FLAG_ANIMATED			(1<<0)

class COutputSceneObject
{
public:
	char pszDefinitionName[ 64 ];
	signed int x;
	signed int y;
	float sort;
	unsigned int flags;
	
	COutputSceneObject()
	{
		memset( pszDefinitionName, 0, sizeof(pszDefinitionName));
		x = 0;
		y = 0;
		sort = 0.0f;
		flags = 0;
	};
};

class COutputScene
{
public:
	unsigned int NumObjects;
	COutputSceneObject Object[];
};





/************************************************************************************************************************************************************************
 
 
 Input data format
 
 
 */

class CTileBank
{
public:
	const char* pszName;
	int FirstTile;
	
	int TileWidth;
	int TileHeight;
	int SrcImageWidth;
	int SrcImageHeight;
	int NumTiles;
	
	bool GetTileIndexInRage( int _tile_id )
	{
		if( _tile_id < FirstTile ) return false;
		if( _tile_id >= FirstTile+NumTiles ) return false;
		return true;
	}
};

class CSpriteDefinition
{
public:
	int tile_id;
	const char* pszName;
	const char* pszAnimationName;
	int w;
	int h;
	int hotspot_x;
	int hotspot_y;
};

class CSpriteInstance
{
public:
	CSpriteDefinition* pSpriteDefinition;
	int x;
	int y;
	float sort;
	
};

class CTileMap
{
public:
	unsigned int Width;
	unsigned int Height;
	const char* TileMapName;
	CTileBank* pTileBank;
	unsigned short* Tiles;
};

class CGameObjectTemplate
{
public:
	int WorldPosX;
	int WorldPosY;
	int flags;
	const char* ImageName;
};

class CScene
{
public:
	list<CTileBank*> TileBanks;
	list<CTileMap*> TileMaps;
	list<CSpriteDefinition*> SpriteDefinitions;
	vector<CSpriteInstance*> Sprites;
	int NumGameObjects;
	
	float currentSortValue;
	
	CScene()
	{
		currentSortValue = 0.0f;
	}
	
	CTileBank* GetTileBankForTile( int _tile_id )
	{
		list<CTileBank*>::iterator i;
		for( i=TileBanks.begin(); i != TileBanks.end(); i++ )
		{
			CTileBank* pTileBank = *i;
			if(pTileBank->GetTileIndexInRage( _tile_id ))
			   return pTileBank;
		}
		
		return NULL;
	}
	
	CSpriteDefinition* GetSpriteDefinitionForTile( int _tile_id )
	{
		list<CSpriteDefinition*>::iterator i;
		for( i=SpriteDefinitions.begin(); i != SpriteDefinitions.end(); i++ )
		{
			CSpriteDefinition* pSpriteDefinition = *i;
			if( pSpriteDefinition->tile_id == _tile_id )
				return pSpriteDefinition;
		}
		
		return NULL;
	};
};

const char* GetTileMapSymbol( const char* _pszSymbol )
{
	return _pszSymbol;
}

char symbol[ 1024 ];

const char* GetTilesSymbol( const char* _pszSymbol )
{
	sprintf( symbol, "%s_tiles", _pszSymbol );
	return (const char*)symbol;
}

const char* GetSceneSymbol( const char* _pszSymbol )
{
	sprintf( symbol, "%s_scene", _pszSymbol );
	return (const char*)symbol;
}

CScene* ParseMapElement( const XMLElement* _pNode );

CTileBank* ParseTilebankElement( const XMLElement* _pTileBankRootElement, const XMLElement* _pImageElement )
{
	CTileBank* pRet = new CTileBank();
	pRet->FirstTile = _pTileBankRootElement->IntAttribute( "firstgid" );
	pRet->TileWidth = _pTileBankRootElement->IntAttribute( "tilewidth" );
	pRet->TileHeight = _pTileBankRootElement->IntAttribute( "tileheight" );
	pRet->SrcImageWidth = _pImageElement->IntAttribute( "width" );
	pRet->SrcImageHeight = _pImageElement->IntAttribute( "height" );
	pRet->pszName = _pImageElement->Attribute( "source" );
	
	int TileW = pRet->SrcImageWidth / pRet->TileWidth;
	int TileH = pRet->SrcImageHeight / pRet->TileHeight;
	pRet->NumTiles = TileW*TileH;
	
	//printf("Tilebank element. Source=%s, first id=%i, tile width=%i height=%i, src image width=%i height=%i, num tiles=%i\n", pRet->pszName, pRet->FirstTile, pRet->TileWidth, pRet->TileHeight, pRet->SrcImageWidth, pRet->SrcImageHeight, pRet->NumTiles);

	return pRet;
}

int GetIntProperty( const char* _pszPropertyName, const XMLElement* _pProperties, int _defaultValue = 0 )
{
	//
	const XMLElement* pPropertyElement = _pProperties->FirstChildElement();
	while(pPropertyElement != NULL)
	{
		const char* pszPropName = pPropertyElement->Attribute( "name" );
		if( !strcmp( pszPropName, _pszPropertyName ))
			return pPropertyElement->IntAttribute( "value" );
		
		pPropertyElement = pPropertyElement->NextSiblingElement();
	}
	
	return _defaultValue;
}

const char* GetStringProperty( const char* _pszPropertyName, const XMLElement* _pProperties, const char* _defaultValue = NULL )
{
	//
	const XMLElement* pPropertyElement = _pProperties->FirstChildElement();
	while(pPropertyElement != NULL)
	{
		const char* pszPropName = pPropertyElement->Attribute( "name" );
		if( !strcmp( pszPropName, _pszPropertyName ))
			return pPropertyElement->Attribute( "value" );
		
		pPropertyElement = pPropertyElement->NextSiblingElement();
	}
	
	return _defaultValue;
}



void ParseSpriteDefinitionProperties( CSpriteDefinition* _pRet, const XMLElement* _pProperties )
{
	_pRet->hotspot_x = GetIntProperty( "hotspot_x", _pProperties, 0 );
	_pRet->hotspot_y = GetIntProperty( "hotspot_y", _pProperties, 0 );
	_pRet->pszAnimationName = GetStringProperty( "animation", _pProperties, NULL );
	printf("Animation: %s\n", _pRet->pszAnimationName);
}

CSpriteDefinition* ParseSpriteDefinitionElement( const XMLElement* _pTileBankRootElement, const XMLElement* _pImageElement )
{
	CSpriteDefinition* pRet = new CSpriteDefinition();

	//
	int tile_id = _pImageElement->IntAttribute( "id" );
	int first_gid = _pTileBankRootElement->IntAttribute( "firstgid" );
	pRet->tile_id = first_gid+tile_id;
	
	//
	const XMLElement* pChildElement = _pImageElement->FirstChildElement();
	while(pChildElement != NULL)
	{
		if( strcmp( "properties", pChildElement->Name()) == 0 )
		{
			ParseSpriteDefinitionProperties( pRet, pChildElement );
		} else if( strcmp( "image", pChildElement->Name()) == 0 )
		{
			pRet->pszName = pChildElement->Attribute( "source" );
			pRet->w = pChildElement->IntAttribute( "width" );
			pRet->h = pChildElement->IntAttribute( "height" );
		}
		
		
		pChildElement = pChildElement->NextSiblingElement();
	}

	//printf("name='%s', first id=%i, tile id=%i (that makes its global id %i) hotspot x=%i y=%i\n", pRet->pszName, first_gid, tile_id, first_gid+tile_id, pRet->hotspot_x, pRet->hotspot_y );
	
	return pRet;
}

void ReadTileSet( const XMLElement* _pElement, CScene* pRetScene )
{
	//printf("ROOT ELEMENT: %s\n", _pElement->Name());
	const XMLElement* pTileSetEntry = _pElement->FirstChildElement();
	while(pTileSetEntry != NULL)
	{
		//printf("Tileset element: %s\n", pTileSetEntry->Name());
		if( strcmp( "image", pTileSetEntry->Name()) == 0 )
		{
			//
			// This is a tilebank
			//
			CTileBank* pNewTileBank = ParseTilebankElement( _pElement, pTileSetEntry );
			pRetScene->TileBanks.push_back( pNewTileBank );
		} else if( strcmp( "tile", pTileSetEntry->Name()) == 0 )
		{
			//
			// This is a sprite/object definition
			//
			CSpriteDefinition* pSpriteDefinition = ParseSpriteDefinitionElement( _pElement, pTileSetEntry );
			pRetScene->SpriteDefinitions.push_back( pSpriteDefinition );
		}
		
		
		pTileSetEntry = pTileSetEntry->NextSiblingElement();
	}
}

unsigned short* ParseLayerCSV( CScene* _pRetScene, CTileBank** _ppRetTileBank, const char* _pszLayerCSV )
{
	//printf("ParseLayerCSV data: %s\n", _pszLayerCSV );

	vector<int> retVector;

	std::string str = _pszLayerCSV;
	std::stringstream ss(str);
	
	unsigned int i;
	
	while (ss >> i)
	{
		//printf("i=%i\n", i );
		retVector.push_back(i);
		
		if((ss.peek() == ',') || (ss.peek() == 0x0a))
			ss.ignore();
	}
	
	*_ppRetTileBank = NULL;
	
	int size = (int)retVector.size();
	//printf("return size=%i\n", size );
	
	unsigned short* pret = new unsigned short[ size ];
	
	for( i=0; i<size; i++ )
	{
		unsigned int r = retVector[ i ];
		unsigned short w = 0;
		unsigned int tileindex = r & 0x0fffffff;
		bool flipX = (r & 0x80000000) == 0x80000000;
		bool flipY = (r & 0x40000000) == 0x40000000;
		bool flipD = (r & 0x20000000) == 0x20000000;
		w = tileindex & 0x1fff;
		if( flipX ) w |= 0x8000;
		if( flipY ) w |= 0x4000;
		if( flipD ) w |= 0x2000;
		//printf( "tile %i=0x%04x\n", i, w );
		pret[ i ] = w;
		
		if( *_ppRetTileBank == NULL )
		{
			CTileBank* pTileBank = _pRetScene->GetTileBankForTile( tileindex );
			if( pTileBank != NULL )
				*_ppRetTileBank = pTileBank;
		}
	}
	
	return pret;
}

CTileMap* ParseLayerElement( CScene* _pRetScene, const XMLElement* _pElement )
{
	CTileMap* ret = new CTileMap();

	//printf("element=%s\n", _pElement->Value());
	ret->Width = _pElement->IntAttribute( "width" );
	ret->Height = _pElement->IntAttribute( "height" );
	//printf( "Map width=%i, height=%i\n", ret->Width, ret->Height );
	
	const XMLElement* dataElement = _pElement->FirstChildElement();
	ret->Tiles = ParseLayerCSV( _pRetScene, &ret->pTileBank, dataElement->GetText());

	return ret;
}

void ParseObjectGroupProperties( CScene* _pRet, const XMLElement* _pProperties )
{
	_pRet->currentSortValue = GetIntProperty( "sort", _pProperties, 0 );
}


void ParseObjectGroup( CScene* _pRet, const XMLElement* _pObjectgroupElement )
{
	
	const XMLElement* child = _pObjectgroupElement->FirstChildElement();
	while( child != NULL )
	{
		if( !strcmp( "object", child->Name()))
		{
			CSpriteInstance* pSpriteInstance = new CSpriteInstance();
			pSpriteInstance->pSpriteDefinition = _pRet->GetSpriteDefinitionForTile( child->IntAttribute( "gid" ));
			pSpriteInstance->x = child->IntAttribute( "x" );
			pSpriteInstance->y = child->IntAttribute( "y" );
			pSpriteInstance->sort = _pRet->currentSortValue;
			_pRet->currentSortValue += 0.01f;
			_pRet->Sprites.push_back( pSpriteInstance );
			//printf( "Sprite x=%i, y=%i, sort=%.2f, definition=%s\n", pSpriteInstance->x, pSpriteInstance->y, pSpriteInstance->sort, pSpriteInstance->pSpriteDefinition->pszName );
		} else if( !strcmp( "properties", child->Name()))
		{
			ParseObjectGroupProperties( _pRet, child );
		}
		child = child->NextSiblingElement();
	}
}


CScene* ParseMapElement( const XMLElement* _pElement )
{
	//printf("Parsing map node, woop!\n");

	CScene* ret = new CScene();
	
	const XMLElement* child = _pElement->FirstChildElement();
	while( child != NULL )
	{
		//printf("Child: %s\n", child->Name());

		if( 0 == strcmp( "tileset", child->Name()))
			ReadTileSet( child, ret );
		
		if( 0 == strcmp( "layer", child->Name()))
		{
			CTileMap* pTileMap = ParseLayerElement( ret, child );
			ret->TileMaps.push_back( pTileMap );
		}
		
		if( 0 == strcmp( "objectgroup", child->Name()))
		{
			ParseObjectGroup( ret, child );
		}

		child = child->NextSiblingElement();
	}
	
	return ret;
}

/*
void WriteScene( FILE* _pFile, CScene* _pScene, const char* _pszSymbolBase )
{
	fprintf( _pFile, "CScene %s = \n" );
	fprintf( _pFile, "{\n" );
}
*/

void WriteCHeader( FILE* _pFile, const char* _pszInFileName, const char* _pszSymbolBase )
{
	fprintf( _pFile, "//\n" );
	fprintf( _pFile, "// Data file generated from %s\n", _pszInFileName );
	fprintf( _pFile, "//\n" );
	fprintf( _pFile, "\n" );
	fprintf( _pFile, "#include \"Engine/Types.h\"\n" );
	fprintf( _pFile, "#include \"Engine/Graphics/TileMap.h\"\n" );
	fprintf( _pFile, "\n" );
}

void WriteTiles( FILE* _pFile, CTileMap* _pTileMap, const char* _pszSymbolBase, int* _pTotalOutputSize )
{
	fprintf( _pFile, "const uint16 %s[] =\n", GetTilesSymbol( _pszSymbolBase ));
	fprintf( _pFile, "{\n" );
	int x, y;
	int i = 0;
	for( y=0; y<_pTileMap->Height; y++ )
	{
		fprintf( _pFile, "\t" );
		for( x=0; x<_pTileMap->Width; x++ )
		{
			fprintf( _pFile, "%i,", _pTileMap->Tiles[ i ]-1);
			*_pTotalOutputSize += 2;
			i++;
		}
		fprintf( _pFile, "\n" );
	}
	fprintf( _pFile, "};\n" );
	fprintf( _pFile, "\n" );
}

void WriteTileMap( FILE* _pFile, CTileMap* _pTileMap, const char* _pszSymbolBase )
{
	fprintf( _pFile, "extern \"C\" const CTileMap %s;\n", _pszSymbolBase );
	fprintf( _pFile, "const CTileMap %s =\n", _pszSymbolBase );
	fprintf( _pFile, "{\n" );
	fprintf( _pFile, "\t%i,%i,\n", _pTileMap->Width, _pTileMap->Height );
	fprintf( _pFile, "\t0,\n" ); // Padding
	fprintf( _pFile, "\t(uint16*)&%s,\n", GetTilesSymbol( _pszSymbolBase ));
	fprintf( _pFile, "};\n" );
	fprintf( _pFile, "\n" );
}

void ExportMaps( CScene* _pInputScene, const char* _pszOutputFileName )
{
	char pszFinalName[ 1024 ];
	sprintf( pszFinalName, "%s.pest", _pszOutputFileName );

	
	FILE* f = fopen(pszFinalName, "wb" );
	
	fclose(f);
}

void ExportSceneObjects( CScene* _pOutputScene, const char* _pszOutFileName )
{
	char pszFinalName[ 1024 ];
	sprintf( pszFinalName, "%s.peso", _pszOutFileName );
	
	FILE* f = fopen(pszFinalName, "wb" );
	
	unsigned int numObjects = (unsigned int)_pOutputScene->Sprites.size();
	//fwrite( &numObjects, 1, 4, f );
	
	COutputSceneObject OutputSceneObject;
	int i;
	for( i=0; i<numObjects; i++ )
	{
		COutputSceneObject* pOutputObject = &OutputSceneObject;
		CSpriteInstance* pInputObject = _pOutputScene->Sprites[ i ];
		
		pOutputObject->flags = 0;
		pOutputObject->x = pInputObject->x + pInputObject->pSpriteDefinition->hotspot_x;
		pOutputObject->y = pInputObject->y + pInputObject->pSpriteDefinition->hotspot_y - pInputObject->pSpriteDefinition->h;
		pOutputObject->sort = pInputObject->sort;
		
		if( pInputObject->pSpriteDefinition->pszAnimationName != NULL )
		{
			//
			// Object should be animated
			//
			strcpy( pOutputObject->pszDefinitionName, pInputObject->pSpriteDefinition->pszAnimationName	);
			pOutputObject->flags |= OSO_FLAG_ANIMATED;
			/*
			printf("Animated item '%s' was at %i,%i with hotspot %i,%i, so end position become %i,%i\n",
				   pInputObject->pSpriteDefinition->pszAnimationName,
				   pInputObject->x, pInputObject->y,
				   pInputObject->pSpriteDefinition->hotspot_x, pInputObject->pSpriteDefinition->hotspot_y,
				   pOutputObject->x, pOutputObject->y );
			 */
		} else
		{
			//
			// Object should be static
			//
			strcpy( pOutputObject->pszDefinitionName, pInputObject->pSpriteDefinition->pszName );
			pOutputObject->pszDefinitionName[ strlen( pOutputObject->pszDefinitionName )-1 ] = 0;
			pOutputObject->pszDefinitionName[ strlen( pOutputObject->pszDefinitionName )-1 ] = 0;
			pOutputObject->pszDefinitionName[ strlen( pOutputObject->pszDefinitionName )-1 ] = 0;
			pOutputObject->pszDefinitionName[ strlen( pOutputObject->pszDefinitionName )-1 ] = 0;
		}
		fwrite( pOutputObject, sizeof(COutputSceneObject), 1, f );
	}
	
	fclose(f);
}


int main( int _numArgs, const char * _apszArg[])
{
	if( _numArgs != 3 )
	{
		printf("ERROR:\n");
		printf("  tmx2c <infile.tmx> <base_outfile_name>\n");
		return -1;
	}
	
	const char* pszInFileName = _apszArg[ 1 ];
	const char* pszOutFileNameBase = _apszArg[ 2 ];
	//const char* pszOutSymbolName = _apszArg[ 3 ];
	
	XMLDocument mapDocument;
	mapDocument.LoadFile( pszInFileName );
	
	if( mapDocument.Error())
	{
		printf("Load error in file %s\n", pszInFileName );
		printf("Error: %s\n", mapDocument.ErrorName());
		return -2;
	}
	
	CScene* scene = NULL;
	const XMLElement* element = mapDocument.FirstChildElement();
	while( element != NULL )
	{
		//printf( "element: %s\n", element->Value());
		if( strcmp( "map", element->Value()) == 0 )
		{
			//
			// This is the map node
			//
			scene = ParseMapElement( element );
		}
		element = element->NextSiblingElement();
	}

	if(scene == NULL)
	{
		printf("ERROR ERROR\n");
		return -1;
	}
	
	//FILE* f;

	//int totalOutputSize = 0;
	
	//
	// Write C file
	//
	//f = OpenCFile( pszOutFileNameBase );
	//WriteCHeader( f, pszInFileName, pszOutSymbolName );
	//WriteTiles( f, scene->TileMap, pszOutSymbolName, &totalOutputSize );
	//WriteTileMap( f, scene->TileMap, pszOutSymbolName );
	//fclose( f );

	//
	// Write header file
	//
	//f = OpenHFile( pszOutFileNameBase );
	//WriteHFile( f, pszInFileName, pszOutSymbolName );
	//fclose( f );

	ExportSceneObjects( scene, pszOutFileNameBase );
	ExportMaps( scene, pszOutFileNameBase );
	
	//printf( "Total output size: %i\n", totalOutputSize );
	return 0;
}
