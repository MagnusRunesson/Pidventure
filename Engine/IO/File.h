//
//  File.h
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-06.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#ifndef File_h
#define File_h

void fileInit( const char* _pszGameIdentifier );
bool fileLoad( const char* _pszFileName, void** _ppReadData, int* _pReadBytes );
bool fileLoad( const char* _pszFileName, void* _pReadDestination, int _bufferSize, int* _pReadBytes );
void fileSave( const char* _pszFileName, void* _pContent, int _contentSize );
void fileUnload( void* _ptr );
const char* fileTranslatorGetCrunchedName( const char* _pszHumanReadable );

#endif /* File_h */
