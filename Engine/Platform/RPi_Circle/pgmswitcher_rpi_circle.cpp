//
//  pgmswitcher_osx.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2017-10-13.
//  Copyright © 2017 Magnus Runesson. All rights reserved.
//

#include "Engine/IO/File.h"

#define TEMPLATE_FILENAME "config.tpl"
#define CONFIG_FILENAME "config.txt"

void pgmSwitcherReset()
{
	// Replace content in config.txt with what's in config.tpl
	unsigned char* pConfigTemplateData;
	int configTemplateLength;
	fileLoad( TEMPLATE_FILENAME, (void**)&pConfigTemplateData, &configTemplateLength );
	fileSave( CONFIG_FILENAME, pConfigTemplateData, configTemplateLength );
}

void pgmSwitcherSwitch( const char* _pszNewProgramIdentifier )
{
	// In data
	unsigned char* pConfigTemplateData;
	int configTemplateLength;
	fileLoad( TEMPLATE_FILENAME, (void**)&pConfigTemplateData, &configTemplateLength );

	// Out data
	int newConfigDataLength = configTemplateLength + 16;	// 16 is chosen because it will fit "\nkernel=xxx.img\n"
	unsigned char* pNewConfigData = new unsigned char[ newConfigDataLength ];
	int i;
	for( i=0; i<configTemplateLength; i++ )
		pNewConfigData[ i ] = pConfigTemplateData[ i ];
	
	pNewConfigData[ i++ ] = '\n';
	pNewConfigData[ i++ ] = 'k';
	pNewConfigData[ i++ ] = 'e';
	pNewConfigData[ i++ ] = 'r';
	pNewConfigData[ i++ ] = 'n';
	pNewConfigData[ i++ ] = 'e';
	pNewConfigData[ i++ ] = 'l';
	pNewConfigData[ i++ ] = '=';
	pNewConfigData[ i++ ] = _pszNewProgramIdentifier[ 0 ];
	pNewConfigData[ i++ ] = _pszNewProgramIdentifier[ 1 ];
	pNewConfigData[ i++ ] = _pszNewProgramIdentifier[ 2 ];
	pNewConfigData[ i++ ] = '.';
	pNewConfigData[ i++ ] = 'i';
	pNewConfigData[ i++ ] = 'm';
	pNewConfigData[ i++ ] = 'g';
	pNewConfigData[ i++ ] = '\n';
	
	fileSave( CONFIG_FILENAME, pNewConfigData, newConfigDataLength );
}
