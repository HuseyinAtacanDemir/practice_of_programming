#include <stdio.h>
#include <stdlib.h>

#ifdef DEBUG
typedef struct {
    long long allocated_bytes;
    int n_invoked;
    FILE *ferr;
} D_ALLOC;

static D_ALLOC dalloc = {0, 0, NULL}; // Initialize with NULL for ferr pointer
#endif

void *dmalloc(int size);
void dfree(void *ptr);

int main(void)
{
    #ifdef DEBUG
    dalloc.ferr = fopen("log.txt", "a");
    if (dalloc.ferr == NULL) {
        perror("Failed to open log file");
        return 1;
    }
    setbuf(dalloc.ferr, NULL); // Disable buffering
    #endif

    char *str = (char *)dmalloc(sizeof(char) * 10);
    int i;
    for (i = 0; i < 9; i++)
        str[i] = 'a'+i;
    str[i] = '\0';
    printf("%s\n", str);

    dfree(str);

    #ifdef DEBUG
    fclose(dalloc.ferr); // Close log file when done
    #endif

    return 0;
}

#ifdef DEBUG
void *dmalloc(int size)
{
    int n_invoked = dalloc.n_invoked++;
    void *block;

    fprintf(dalloc.ferr, "%d, Requested: %d bytes\n", n_invoked, size);

    block = malloc(size);

    if (block == NULL) 
        fprintf(dalloc.ferr, "%d, NOMEM ERR\n", n_invoked);
    else
        fprintf(dalloc.ferr, "%d, Allocated: %d bytes at %p\n", n_invoked, size, block);

    return block;
}

void dfree(void *ptr)
{
    fprintf(dalloc.ferr, "freeing %p\n", ptr);
    free(ptr);
}
#else
void *dmalloc(int size)
{
    return malloc(size);
}

void dfree(void *ptr)
{
    free(ptr);
}
#endif
