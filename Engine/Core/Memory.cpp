#define MEMORY_TRACKING	(0)

//
//  Memory.cpp
//  pidventure
//
//  Created by Magnus Runesson on 2016-10-31.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#if MEMORY_TRACKING == 1
#include <thread>
#include <stdio.h>
#endif

#include "Engine/Core/Memory.h"
#include "Engine/Core/Debug.h"

#undef new
#undef delete

#if MEMORY_TRACKING == 1

static std::__thread_id trackedThread;
static FILE* g_fileNew;
static FILE* g_fileDelete;

void memTrackInit()
{
	trackedThread = std::this_thread::get_id();
	
	g_fileNew = fopen("memtrack_new.txt", "w");
	g_fileDelete = fopen("memtrack_del.txt", "w");
}

void memTrackExit()
{
	fclose(g_fileNew);
	fclose(g_fileDelete);
}

void memTrackAlloc(void* _ptr, size_t _size, const char* _pszFileName, int _line)
{
	if(trackedThread == std::this_thread::get_id())
	{
		fprintf( g_fileNew, "0x%016llx,%s,%i,%u\n", (unsigned long long)_ptr, _pszFileName, _line, (unsigned int)_size );
		//debugLog("------------------> Calling new from %s:%i - returning 0x%08x\n", _pszFileName, 0, _ptr );
	}
}

void memTrackDelete(void* _ptr)
{
	if(trackedThread == std::this_thread::get_id())
	{
		fprintf( g_fileDelete, "0x%016llx\n", (unsigned long long)_ptr );
	}
}
#else

#define memTrackInit(a)
#define memTrackExit(a)
#define memTrackAlloc(a,b,c,d)
#define memTrackDelete(a)

#endif


void memInit()
{
	memTrackInit();
}

void memExit()
{
	memTrackExit();
}

#ifndef ENGINE_TARGET_RPI_CIRCLE

void* operator new(size_t size)
{
	void* ptr = malloc(size);
	memTrackAlloc(ptr, size, "<unknown>", 0);
	return ptr;
}

void* operator new[](size_t size)
{
	void* ptr = malloc(size);
	memTrackAlloc(ptr, size, "<unknown>", 0);
	return ptr;
}

void* operator new(size_t size, const char* _pszFile, int _line)
{
	void* ptr = malloc(size);
	memTrackAlloc(ptr, size, _pszFile, _line);
	return ptr;
}

void* operator new[](size_t size, const char* _pszFile, int _line)
{
	void* ptr = malloc(size);
	memTrackAlloc(ptr, size, _pszFile, _line);
	return ptr;
}

void operator delete(void* ptr) throw()
{
	memTrackDelete(ptr);
	free(ptr);
}

void operator delete[](void* ptr) throw()
{
	memTrackDelete(ptr);
	free(ptr);
}

#endif
