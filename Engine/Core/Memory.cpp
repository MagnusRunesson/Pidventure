//
//  Memory.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2016-10-31.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#include <thread>
#include <stdio.h>
#include "Engine/Core/Memory.h"
#include "Engine/Core/Debug.h"

#undef new
#undef delete

static std::__thread_id trackedThread;
static FILE* g_fileNew;
static FILE* g_fileDelete;

void memTrackAlloc(void* _ptr, size_t _size, const char* _pszFileName, int _line)
{
	fprintf( g_fileNew, "0x%016llx,%s,%i,%u\n", (unsigned long long)_ptr, _pszFileName, _line, (unsigned int)_size );
}

void memTrackDelete(void* _ptr)
{
	fprintf( g_fileDelete, "0x%016llx\n", (unsigned long long)_ptr );
}

void memInit()
{
	trackedThread = std::this_thread::get_id();
	
	g_fileNew = fopen("memtrack_new.txt", "w");
	g_fileDelete = fopen("memtrack_del.txt", "w");
}

void memExit()
{
	fclose(g_fileNew);
	fclose(g_fileDelete);
}

void* operator new(size_t size)
{
	void* ptr = malloc(size);
	if(trackedThread == std::this_thread::get_id())
	{
		memTrackAlloc(ptr, size, "<unknown>", 0);
		debugLog("------------------> Calling new from %s:%i - returning 0x%08x\n", "<unknown>", 0, ptr );
	}
	return ptr;
}

void* operator new[](size_t size)
{
	void* ptr = malloc(size);
	debugLog("Main thread=%i, this thread=%i\n", trackedThread, std::this_thread::get_id());
	
	if(trackedThread == std::this_thread::get_id())
	{
		memTrackAlloc(ptr, size, "<unknown>", 0);
		debugLog("------------------> Calling new[] from %s:%i - returning 0x%08x\n", "<unknown>", 0, ptr );
	}
	return ptr;
}

void* operator new(size_t size, const char* _pszFile, int _line)
{
	void* ptr = malloc(size);
	if(trackedThread == std::this_thread::get_id())
	{
		memTrackAlloc(ptr, size, _pszFile, _line);
		debugLog("------------------> Calling new from %s:%i - returning 0x%08x\n", _pszFile, _line, ptr );
	}
	return ptr;
}

void* operator new[](size_t size, const char* _pszFile, int _line)
{
	void* ptr = malloc(size);
	if(trackedThread == std::this_thread::get_id())
	{
		memTrackAlloc(ptr, size, _pszFile, _line);
		debugLog("------------------> Calling new[] from %s:%i - returning 0x%08x\n", _pszFile, _line, ptr );
	}
	return ptr;
}

void operator delete(void* ptr) throw()
{
	if(trackedThread == std::this_thread::get_id())
	{
		memTrackDelete(ptr);
		debugLog("------------------> Calling delete on 0x%08x\n", ptr );
	}
	free(ptr);
}

void operator delete[](void* ptr) throw()
{
	if(trackedThread == std::this_thread::get_id())
	{
		memTrackDelete(ptr);
		debugLog("------------------> Calling delete[] on 0x%08x\n", ptr );
	}
	free(ptr);
}
