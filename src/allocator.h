#ifndef ALLOCATOR
#define ALLOCATOR

#include <stddef.h>

void *myalloc(size_t size);
void release_block(void *ptr);

#endif
