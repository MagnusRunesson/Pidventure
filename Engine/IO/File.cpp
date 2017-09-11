#include "Engine/Core/Debug.h"
#include "Engine/IO/File.h"
#include "Engine/Util/String.h"

typedef struct {
	char* pszHumanReadableName;
	char* pszCrunchedName;
} ftEntry;

int fileTranslatorTableNumEntries;
ftEntry fileTranslatorTable[ 400 ];

char itoabuffer[ 10 ];
char* itoa( int _value )
{
	int outoffset = 0;
	
	if( _value > 999 )
	{
		int v = (_value / 1000) % 10;
		itoabuffer[ outoffset ] = '0' + v;
		outoffset++;
	}

	if( _value > 99 )
	{
		int v = (_value / 100) % 10;
		itoabuffer[ outoffset ] = '0' + v;
		outoffset++;
	}

	if( _value > 9 )
	{
		int v = (_value / 10) % 10;
		itoabuffer[ outoffset ] = '0' + v;
		outoffset++;
	}

	{
		int v = _value % 10;
		itoabuffer[ outoffset ] = '0' + v;
		outoffset++;
	}
	
	itoabuffer[ outoffset ] = 0;
	return itoabuffer;
}

void fileInit( const char* _pszGameIdentifier )
{
	debugLog( "FILE INIT!!!\n" );
	
	fileTranslatorTableNumEntries = 0;
	
	char pszFileName[ 13 ];
	pszFileName[ 0 ] = _pszGameIdentifier[ 0 ];
	pszFileName[ 1 ] = _pszGameIdentifier[ 1 ];
	pszFileName[ 2 ] = _pszGameIdentifier[ 2 ];
	pszFileName[ 3 ] = 'f';
	pszFileName[ 4 ] = 'i';
	pszFileName[ 5 ] = 'l';
	pszFileName[ 6 ] = 'e';
	pszFileName[ 7 ] = 's';
	pszFileName[ 8 ] = '.';
	pszFileName[ 9 ] = 't';
	pszFileName[ 10 ] = 'x';
	pszFileName[ 11 ] = 't';
	pszFileName[ 12 ] = 0;
	
	void* pFileListData;
	int numBytesRead;
	fileLoad( pszFileName, &pFileListData, &numBytesRead );
	
	unsigned char* pData = (unsigned char*)pFileListData;
	
	unsigned char sourceName[ 1024 ];
	unsigned char dstName[ 13 ];
	
	//debugLog("Parsing file\n");
	fileTranslatorTableNumEntries = 0;
	int i = 0;
	//debugLog( "Size of filelist.txt: %d\n", numBytesRead );
	
	while( i<numBytesRead )
	{
		// Read source name
		int readChar = 0;
		unsigned char c;
		while(( c=pData[ i ]) != ':' )
		{
			sourceName[ readChar ] = c;
			i++;
			readChar++;
		}
		
		sourceName[ readChar ] = 0;
		i++; // Skip :
		
		readChar = 0;
		while((( c=pData[ i ]) != 0x0a) && (i<numBytesRead))
		{
			dstName[ readChar ] = c;
			i++;
			readChar++;
		}
		
		dstName[ readChar ] = 0;
		i++; // Skip new line
		
		debugLog("Line %i: %s = %s", fileTranslatorTableNumEntries, sourceName, dstName );
		
		fileTranslatorTable[ fileTranslatorTableNumEntries ].pszHumanReadableName = stringDuplicate( (const char*)sourceName );
		fileTranslatorTable[ fileTranslatorTableNumEntries ].pszCrunchedName = stringDuplicate( (const char*)dstName );

		fileTranslatorTableNumEntries++;
	}
	
	fileUnload( pFileListData );
	
	debugLog("File init done");
}

const char* fileTranslatorGetCrunchedName( const char* _pszHumanReadable )
{
//	debugLog( "Translating name. Table entries: %i\n", fileTranslatorTableNumEntries );
	
	// No entries in table
	if( fileTranslatorTableNumEntries == 0 )
		return _pszHumanReadable;

	//
	int i;
	for( i=0; i<fileTranslatorTableNumEntries; i++ )
		if( stringCompare( fileTranslatorTable[ i ].pszHumanReadableName, _pszHumanReadable ) == 0 )
		{
//			debugLog( "Human readable: %s, crunched name: %s\n", _pszHumanReadable, fileTranslatorTable[ i ].pszCrunchedName );
			return fileTranslatorTable[ i ].pszCrunchedName;
		}
	
	// Did not crunched file name in table, nooes!
//	debugLog( "Didn't find a crunched name for human readable %s\n", _pszHumanReadable );
	return _pszHumanReadable;
}
