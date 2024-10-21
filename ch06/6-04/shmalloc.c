#include "shmalloc.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>

#define SUCCESS             0
#define ERR_NOT_ALLOCD      1
#define ERR_DOUBLE_FREE     2
#define ERR_NOT_INITIALIZED 3
#define ERR_MMAP            4
#define ERR_PTHREAD         5

#define PAGE_SIZE   sysconf(_SC_PAGESIZE) 
#define HEADER_SIZE sizeof(HEADER)

#define FREE        'F' 
#define USED        'U'
#define RESTRICTED  'R'

// HEADER's with is_free == RESTRICTED point to the actual mapped pages
// returned from mmap, and their size is the actual size mapped by mmap.
// All other HEADER*->size values contain the usable/shmallocable size
typedef struct HEADER HEADER;
struct HEADER {
    HEADER        *next;
    unsigned      size;
    unsigned char is_free;
};

static HEADER *head_p = NULL; 
static HEADER *tail_p = NULL;

static pthread_mutex_t     *lock_p = NULL;
static pthread_mutexattr_t *attr_p = NULL;

// initializing fns
static int  init_shmalloc (size_t size);
static int  init_mutex    (void);

// mem block allocation fns
static void   *get_free_shblock         (size_t size);
static HEADER *get_newly_shmapped_page  (size_t size);

//helper fns
static inline size_t get_page_aligned_min_size (size_t size);
static inline size_t get_next_pow_of_two       (size_t size);

// shmalloc: allocated shared memory block of size "size"
void *shmalloc(size_t size)
{
    void *allocated_block;

    if (size <= 0)
        return NULL;

    if (head_p == NULL)
        if (init_shmalloc(size) != 0)
            return NULL;

    pthread_mutex_lock(lock_p);    
    allocated_block = get_free_shblock(size);
    pthread_mutex_unlock(lock_p);

    return allocated_block;        
}

void *shcalloc(size_t count, size_t size)
{
    void *allocated_block;
    if((allocated_block = shmalloc((size * count))) == NULL)
        return NULL;
    return memset(allocated_block, 0, (size*count));
}

void *shrealloc(void *ptr, size_t size)
{
    HEADER *h;
    void *allocated_block;
    
    if((allocated_block = shmalloc(size)) == NULL)
        return NULL;

    h = (HEADER *) (((char *) ptr) - HEADER_SIZE);
    memcpy(allocated_block, ptr, h->size);
    shfree(ptr);
    ptr = NULL;

    return allocated_block;    
}

// shfree: free shared memory block pointed to by ptr, 
//         coalesce if neighboring blocks are FREE as well
int shfree(void *ptr)
{
    HEADER *h, *cur, *prev;
    
    if (ptr == NULL)
        return SUCCESS;

    pthread_mutex_lock(lock_p);    

    h = (HEADER *) (((char *) ptr) - HEADER_SIZE);
    for (cur = head_p; cur != NULL; prev = cur, cur = cur->next)
        if (cur == h)
            break;

    if (cur == NULL) {
        pthread_mutex_unlock(lock_p); 
        return ERR_NOT_ALLOCD;
    }

    if (cur->is_free == FREE) {
        pthread_mutex_unlock(lock_p); 
        return ERR_DOUBLE_FREE;
    }

    cur->is_free = FREE;
    
    // coalesce cur with prev and next if possible
    if (prev != NULL && prev->is_free == FREE) {
        prev->next = cur->next;
        prev->size += cur->size + HEADER_SIZE;
        cur = prev; // to further coalesce, if next is also free
    }
    if (cur->next != NULL && cur->next->is_free == FREE) {
        cur->size += cur->next->size + HEADER_SIZE;
        cur->next = cur->next->next;
    }

    pthread_mutex_unlock(lock_p); 
   
    return SUCCESS;
}

// shfree_all: free all resources, including bookkeeping data
int shfree_all()
{
    HEADER *h, *prev_page;

    if (head_p == NULL)
        return ERR_NOT_INITIALIZED;
    // TODO:  make sure you can recover from failed munmap in order to be 
    //        able to free at least other resources
    for (h = head_p->next, prev_page = head_p; h != NULL; h = h->next) {
        if (h->is_free == RESTRICTED) {
            if (munmap(prev_page, prev_page->size))
                return ERR_MMAP;
            prev_page = h;
        }
    }
    if (prev_page != NULL && munmap(prev_page, prev_page->size))
        return ERR_MMAP;

    pthread_mutex_destroy(lock_p); 
    if (munmap(lock_p, sizeof(pthread_mutex_t)))
        return ERR_MMAP;
    if (munmap(attr_p, sizeof(pthread_mutexattr_t)))
        return ERR_MMAP;
    
    head_p = NULL;
    lock_p = NULL;
    attr_p = NULL;

    return SUCCESS;
}

// init_shmalloc: initialize the bookkeeping data and mutex locks
static int init_shmalloc(size_t size)
{
    int err;
    if ((err = init_mutex()) != 0)
        return err;

    if ((head_p = get_newly_shmapped_page(size)) == NULL)
        return ERR_MMAP;
    
    tail_p = head_p->next;

    return SUCCESS; 
}

// init_mutex: initialize the pthread_mutex_t used by the shfree and shmalloc
static int init_mutex()
{
    int err = 0;

    lock_p = (pthread_mutex_t *) mmap(NULL, sizeof(pthread_mutex_t), 
                                      PROT_READ | PROT_WRITE, 
                                      MAP_ANON | MAP_SHARED, -1, 0);
    if (lock_p == MAP_FAILED){
        lock_p = NULL;
        return ERR_MMAP;
    }
    
    attr_p = (pthread_mutexattr_t *) mmap(NULL, sizeof(pthread_mutexattr_t),
                                          PROT_READ | PROT_WRITE,
                                          MAP_ANON | MAP_SHARED, -1, 0);
    if (attr_p == MAP_FAILED) {
        munmap(lock_p, sizeof(pthread_mutex_t));
        lock_p = NULL;
        attr_p = NULL;
        return ERR_MMAP;
    }

    if (pthread_mutexattr_init(attr_p))
        err = ERR_PTHREAD;
    else if (pthread_mutexattr_setpshared(attr_p, PTHREAD_PROCESS_SHARED)) {
        pthread_mutexattr_destroy(attr_p);
        err = ERR_PTHREAD;
    } else if (pthread_mutex_init(lock_p, attr_p)) {
        pthread_mutexattr_destroy(attr_p);
        err = ERR_PTHREAD;
    }

    if (err) {
        munmap(lock_p, sizeof(pthread_mutex_t));
        munmap(attr_p, sizeof(pthread_mutexattr_t));
        lock_p = NULL;
        attr_p = NULL;
        return err;
    }

    // attribute itself is destroyed after mutex is initialized with the attr
    pthread_mutexattr_destroy(attr_p);

    return SUCCESS;
}

// get_newly_shmapped_page: mmap a new page, format the newly mmapped page to
//                          fit into the HEADER spec, return the RESTRICTED
//                          HEADER *new_page with its next initialized
static HEADER *get_newly_shmapped_page(size_t size)
{
    HEADER *new_page, *free_p;
    size_t init_size;

    init_size = get_page_aligned_min_size(size);
    new_page = (HEADER *) mmap(NULL, init_size, 
                                PROT_READ | PROT_WRITE, 
                                MAP_ANON | MAP_SHARED, -1, 0);
    if (new_page == MAP_FAILED)
        return NULL;

    new_page->size    = init_size;
    new_page->is_free = RESTRICTED;

    free_p            = new_page + 1;
    free_p->next    = NULL;
    free_p->size    = init_size - (HEADER_SIZE * 2);
    free_p->is_free = FREE;

    new_page->next  = free_p;
    return new_page;
}

// get_free_shblock: finds free shared block of memory in the list, if unable
//                    allocates more shared memory with mmap and bookkeeps
static void *get_free_shblock(size_t size)
{
    HEADER *cur, *next;
    
    if (head_p == NULL)
        return NULL;

    for (cur = head_p->next; cur != NULL; cur = cur->next) 
        if (cur->is_free == FREE && size <= cur->size)
            break;

    if (cur == NULL) {
        tail_p->next = get_newly_shmapped_page(size);

        if (tail_p->next == NULL)
            return NULL;

        cur = tail_p->next->next;
        tail_p = cur;
    }
    
    // if cur has enough space for another block
    if ((cur->size - size) > HEADER_SIZE) {
        next = (HEADER *) ((char *)(cur + 1) + size);
        next->is_free = FREE;
        next->size = cur->size - size - HEADER_SIZE;
        next->next = cur->next;
             
        cur->next = next;
        cur->size = size;
    
        if (cur == tail_p)
            tail_p = cur->next;
    }

    cur->is_free = USED;
    
    return (void *)(cur + 1);
}

// get_page_aligned_min_size: calculates the min nbytes necessary to fit data
//                            of size "size" while making sure nbytes % 4k == 0 
static inline size_t get_page_aligned_min_size(size_t size)
{
    return (size < (PAGE_SIZE - (HEADER_SIZE*2))) ? 
            PAGE_SIZE : get_next_pow_of_two(size + (HEADER_SIZE*2));
}

static inline size_t get_next_pow_of_two(size_t s)
{
    s--;        // subtract 1 in case s % 2 was already 0
    s |= s>>1;
    s |= s>>2;
    s |= s>>4;
    s |= s>>8;
    s |= s>>16;
    s |= s>>32;

    return (++s); // add 1 to find the next pow of 2
}

