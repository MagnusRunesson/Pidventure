//
//  main.cpp
//  memanalyze
//
//  Created by Magnus Runesson on 2016-11-01.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include <stdio.h>
#include <string.h>

char* fileReadContent(const char* _pszFileName)
{
	FILE* f = fopen(_pszFileName, "r");
	fseek(f, 0, SEEK_END);
	int size = (int)ftell(f);
	char* pRet = new char[ size+1 ];
	
	fseek(f, 0, SEEK_SET);
	fread(pRet, 1, size, f);
	fclose(f);
	
	pRet[ size ] = 0;
	
	return pRet;
}

char* skipToEndOfLine(char* _pszSource)
{
	// Search for end of line and/or end of file
	while((*_pszSource != 0x00) && (*_pszSource != 0x0a))
		_pszSource++;
	
	// End of file
	if(*_pszSource == 0 )
		return NULL;
	
	_pszSource++;	// Skip the new line if that is what stopped us
	
	// Verify that the is actually another line after the new line
	if(*_pszSource == 0 )
		return NULL;

	return _pszSource;
}

static char pszString[128];

char* getPtr(char* _pszLine)
{
	char* pszOut = &pszString[ 0 ];
	char* pszOrg = _pszLine;
	while(*pszOrg != ',') {
		*pszOut = *pszOrg;
		pszOut++;
		pszOrg++;
	}
	
	*pszOut = 0;
	return pszString;
}

static char pszString2[ 4192 ];
char* getZeroTerminatedLine( char* _pszLine )
{
	char* pszOut = &pszString2[ 0 ];
	char* pszOrg = _pszLine;
	while( *pszOrg != 0x0a ) {
		*pszOut = *pszOrg;
		pszOut++;
		pszOrg++;
	}
	
	*pszOut = 0;
	return pszString2;
}

void checkLeaks(const char* _pszOutFileName, const char* _pszAllocFileName, const char* _pszFreeName )
{
	char* pszNew = fileReadContent(_pszAllocFileName);
	char* pszDel = fileReadContent(_pszFreeName);
	
	FILE* fOut = fopen(_pszOutFileName, "w");
	
	int numLines=0;
	char* pszLine = pszNew;
	while(pszLine)
	{
		char* pszPointer = getPtr(pszLine);
		
		if( strstr( pszDel, pszPointer ) == NULL)
		{
			fprintf(fOut, "%s\n", getZeroTerminatedLine( pszLine ));
			//printf(pszLine);
			//printf("\n");
			//printf("line %i: ", numLines);
			//printf(pszPointer);
			//printf("\n");
			numLines++;
		}
		
		pszLine = skipToEndOfLine( pszLine );
	}
	
	fclose(fOut);
	
	printf("Found %i leaks\n", numLines);
	
	//printf("new=%s\n", pszNew);
	
	delete[] pszNew;
	delete[] pszDel;
}

int main(int argc, const char * argv[])
{
	checkLeaks( "memleaks.txt", "memtrack_new.txt", "memtrack_del.txt");
	checkLeaks( "fileleaks.txt", "filetrack_open.txt", "filetrack_close.txt");
    return 0;
}
