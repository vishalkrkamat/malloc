#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ALIGNMENT 16
#define ALIGN(x) (((x) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))
#define HEADER_SIZE ALIGN(sizeof(block))

typedef struct block {
    size_t size;
    int free;
    struct block *next;
} block;

block *block_list = NULL;

void *myalloc(size_t size);
void *find_free_block(size_t size);
void release_block(void *ptr);
void *current_memory_break();

void *myalloc(size_t size) {
    size_t payload_size = ALIGN(size);

    block *new_block = find_free_block(payload_size);
    if (new_block) {
        new_block->free = 0;
        printf("found free momory\n");
        return (void *)((char *)new_block + HEADER_SIZE);
    }

    void *rawmem = sbrk(HEADER_SIZE + payload_size);
    if (rawmem == (void *)-1) {
        perror("Allocation failed");
        return NULL;
    }

    printf("Allocating new one\n");
    new_block = (block *)rawmem;
    new_block->size = payload_size;
    new_block->free = 0;
    new_block->next = block_list;
    block_list = new_block;

    return (void *)((char *)new_block + HEADER_SIZE);
}

void *find_free_block(size_t size) {

    block *curr = block_list;
    while (curr) {
        if (curr->free && curr->size >= size) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

void release_block(void *ptr) {
    block *mem = (block *)(((char *)ptr) - HEADER_SIZE);
    mem->free = 1;
}

void *current_memory_break() {
    void *p = sbrk(0);
    if (p == (void *)-1) {
        perror("error in sbrk while getting current program break");
        return NULL;
    }
    return p;
}

int main() {

    printf("Current memory break: %p\n", current_memory_break());
    printf("Size of block: %ld\n", sizeof(block));

    double *user_data = (double *)myalloc(sizeof(double));
    *user_data = 39;

    printf("Value: %f\n", *user_data);
    printf("%ld\n", sizeof(*user_data));

    block *header = (block *)(((char *)user_data) - HEADER_SIZE);
    printf("%ld\n", header->size);

    printf("Current memory break: %p\n", current_memory_break());
    release_block(user_data);
    return 0;
}
