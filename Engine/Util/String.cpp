//
//  String.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2017-08-21.
//  Copyright © 2017 Magnus Runesson. All rights reserved.
//

#include "String.h"

void stringCombine( char* _pszOutput, int _maxLength, const char* _pszInput1, const char* _pszInput2 )
{
	int iDst = 0;
	int iSrc = 0;
	const char* pszCurrentInput = _pszInput1;
	
	_maxLength--;	// We need to be able to append a terminating zero
	while( iDst < _maxLength )
	{
		if( pszCurrentInput[ iSrc ] == 0 )
		{
			// Current string has ended
			// Go to next string or end function
			if( pszCurrentInput == _pszInput1 )
			{
				pszCurrentInput = _pszInput2;
				iSrc = 0;
			}
			else
			{
				break;
			}
		}
		else
		{
			// Current string has not ended
			// Keep copying
			_pszOutput[ iDst ] = pszCurrentInput[ iSrc ];
			iDst++;
			iSrc++;
		}
	}
	
	_pszOutput[ iDst ] = 0;
}

int stringLength( const char* _pszString )
{
	int l = 0;
	while( _pszString[ l ] != 0 )
		l++;
	
	return l;
}

void stringCopy( char* _pszDest, const char* _pszSource )
{
	while( *_pszSource )
	{
		*_pszDest = *_pszSource;
		_pszDest++;
		_pszSource++;
	}
}

int stringCompare( const char* _pszA, const char* _pszB )
{
	int strlendiff = stringLength( _pszA ) - stringLength( _pszB );
	
	// Check for diff in string length
	if( strlendiff != 0 )
		return strlendiff;
	
	//
	int i;
	int l = stringLength( _pszA );
	for( i=0; i<l; i++ )
	{
		int a = _pszA[ i ];
		int b = _pszB[ i ];
		if( a != b )
			return a-b;
	}
	
	return 0;
}

char* stringDuplicate( const char* _pszSource )
{
	char* pszRet = new char[ stringLength( _pszSource )];
	stringCopy( pszRet, _pszSource );
	return pszRet;
}
