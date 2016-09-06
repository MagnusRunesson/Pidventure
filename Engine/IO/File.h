//
//  File.h
//  pidventure
//
//  Created by Magnus Runesson on 2016-09-06.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#ifndef File_h
#define File_h

bool fileLoad(const char* _pszFileName, void** _ppReadData, int* _pReadBytes);
bool fileLoad(const char* _pszFileName, void* _pReadDestination, int _bufferSize, int* _pReadBytes);

#endif /* File_h */
