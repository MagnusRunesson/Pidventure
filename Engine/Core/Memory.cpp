//
//  Memory.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2016-10-31.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include "Engine/Core/Memory.h"
#include "Engine/Core/Debug.h"

#undef new
#undef delete

void* operator new(size_t size)
{
	void* ptr = malloc(size);
	//debugLog("------------------> Calling new from %s:%i - returning 0x%08x\n", "<unknown>", -1, ptr );
	return ptr;
}

void* operator new[](size_t size)
{
	void* ptr = malloc(size);
	//debugLog("------------------> Calling new[] from %s:%i - returning 0x%08x\n", "<unknown>", -1, ptr );
	return ptr;
}

void* operator new(size_t size, const char* _pszFile, int _line)
{
	void* ptr = malloc(size);
	//debugLog("------------------> Calling new from %s:%i - returning 0x%08x\n", _pszFile, _line, ptr );
	return ptr;
}

void* operator new[](size_t size, const char* _pszFile, int _line)
{
	void* ptr = malloc(size);
	//debugLog("------------------> Calling new[] from %s:%i - returning 0x%08x\n", _pszFile, _line, ptr );
	return ptr;
}

void operator delete(void* ptr) throw()
{
	//debugLog("------------------> Calling delete on 0x%08x\n", ptr );
	free(ptr);
}

void operator delete[](void* ptr) throw()
{
	//debugLog("------------------> Calling delete[] on 0x%08x\n", ptr );
	free(ptr);
}
