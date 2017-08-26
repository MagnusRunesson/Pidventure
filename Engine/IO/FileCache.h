//
//  FileCache.h
//  pidventure
//
//  Created by Magnus Runesson on 2016-11-04.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#ifndef FileCache_h
#define FileCache_h

bool fileCacheGetFile( const char* _pszFileName, void** _ppData, int* _pSize );
void fileCacheUnreferenceFile( void* _pData );
void fileCacheAddFile( const char* _pszFileName, void* _pFileData, int _fileSize );
void fileCacheReset();
void fileCacheDebugPrintStats();

#endif /* FileCache_h */
