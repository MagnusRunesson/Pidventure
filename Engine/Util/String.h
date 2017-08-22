//
//  String.h
//  pidventure
//
//  Created by Magnus Runesson on 2017-08-21.
//  Copyright © 2017 Magnus Runesson. All rights reserved.
//

#ifndef String_h
#define String_h

void stringCombine( char* _pszOutput, int _maxLength, const char* _pszInput1, const char* _pszInput2 );
int stringLength( const char* _pszString );
void stringCopy( char* _pszDest, const char* _pszSource );
int stringCompare( const char* _pszA, const char* _pszB );
char* stringDuplicate( const char* _pszSource );

#endif /* String_h */
