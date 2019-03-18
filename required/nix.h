#ifndef __NIX_H__
#define __NIX_H__

#include <string.h>
#include <stdlib.h>

void* PlatformAllocate(int size)
{
	void *result = malloc(size);

	return result;
}

void PlatformFree(void* chunk)
{
	free(chunk);
	chunk=NULL;
}

void ClearMemory(void *ptr, int size)
{
	memset(ptr,0,size);
}
#endif
