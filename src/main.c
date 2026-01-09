#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct block {
    size_t size;
    int free;
    struct block *next;
} block;

void *myalloc(size_t size) {

    void *rawmem = sbrk(sizeof(block) + size);
    if (rawmem == (void *)-1) {
        perror("Allocation failed");
    }
    block *b = (block *)rawmem;
    b->size = size;
    b->next = NULL;
    b->free = 1;

    return (void *)(b + 1);
}

void *heap_end() {
    void *p = sbrk(0);
    if (p == (void *)-1) {
        perror("error in sbrk while getting current program break");
    }
    return p;
}

int main() {

    printf("Current program break: %p\n", heap_end());
    printf("Size of block: %ld\n", sizeof(block));

    int *user_data = (int *)myalloc(sizeof(int));
    *user_data = 39;
    printf("Value: %d\n", *user_data);
    printf("%ld\n", sizeof(*user_data));

    block *header = ((block *)user_data) - 1;
    printf("%ld\n", header->size);
    return 0;
}
