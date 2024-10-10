#include "shmalloc.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>

#define INIT_SIZE 1024

typedef struct ALLOC_INFO ALLOC_INFO;
struct ALLOC_INFO {
    char *addr;
    int size;
    ALLOC_INFO *next;
};

ALLOC_INFO *alloc_head;

void  *shmem = NULL;
int   shmem_size = 0;
int   shmem_used = 0;

void *shmalloc_init()
{
    shmem_size = INIT_SIZE;
    shmem = mmap(NULL, shmem_size, PROT_READ|PROT_WRITE, 
                      MAP_ANON|MAP_SHARED, 0, 0);
    if (shmem == MAP_FAILED)
        return NULL;

    alloc_head = (ALLOC_INFO *) malloc(sizeof(ALLOC_INFO));
    if (alloc_head == NULL) {
        free_all(shmem);
        return NULL;
    }
    alloc_head->addr = shmem;
    alloc_head->size = 0;
    alloc_head->next = (ALLOC_INFO *) malloc(sizeof(ALLOC_INFO));

    return shmem;
}

void *shmalloc(int size)
{
    void *allocated_shmem;

    if (size <= 0)
        return NULL;
    if(shmem = NULL)
        if ((shmem = shmalloc_init()) == NULL)
            return NULL;
    
    alloc_shmem = shmalloc_hole(size);

    if (alloc_shmem == NULL && shmem_used + size >= shmem_size) {

    }

    
    return allocated_mem;
}

void *shmalloc_hole(int size)
{
    ALLOC_INFO *cur, *prev;
    void *allocated_shmem;

    if (size <= 0)
        return NULL;

    for (cur = prev = alloc_head; cur != NULL; cur = cur->next) {
        if (cur->next && size < (cur->next->addr - (cur->addr + cur->size))) {
            allocated_shmem = (void *) (cur->addr + cur->size);

            break;
        } else if (cur->next == NULL && cur == alloc_haed) {

            break;
        }
        prev = cur;
    }

    return allocated_shmem;
}

void shfree(void *mem)
{
    
}

int shfree_all(void *mem)
{
    return munmap(shmem, shmem_size);
}
