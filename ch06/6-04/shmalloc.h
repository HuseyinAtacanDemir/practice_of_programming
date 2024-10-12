/*
shmalloc.h

Allocates space similar to malloc, but in shared memory.

Uses mmap and munmap calls under the hood.

Because there is no mremap on DARWIN, the memory is allocated in chunks.

shmalloc looks for holes in the existing chunks of size >= size requested.

When no such hole exists, shmalloc allocated a new chunk with mmap, and a ptr 
to a mem block wihtin the new chunk of the siz_requested is returned.

Chunk data is held alongside the allocated data.

*/

#ifndef SHMALLOC_H
#define SHMALLOC_H

#include <stddef.h>

extern void  *shmalloc   (size_t size);
extern int   shfree      (void *mem);
extern int   shfree_all  (void);

#endif
