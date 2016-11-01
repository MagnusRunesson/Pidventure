//
//  Memory.h
//  pidventure
//
//  Created by Magnus Runesson on 2016-10-31.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#ifndef Memory_h
#define Memory_h

#include <stdlib.h>

void memInit();
void memExit();

void* operator new(size_t size, const char* _pszFile, int _line);
void* operator new[](size_t size, const char* _pszFile, int _line);

#define new new(__FILE__, __LINE__)

#endif /* Memory_h */
