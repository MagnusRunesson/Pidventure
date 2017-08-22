#ifndef __STDC_STDLIB_H
#define __STDC_STDLIB_H

#ifdef ENGINE_TARGET_RPI_CIRCLE

//
//
#include <circle/alloc.h>

typedef unsigned int size_t;

#define NULL 0

#else

#include <stdlib.h>

#endif


#endif // __STDLIB_H
