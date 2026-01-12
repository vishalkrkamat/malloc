#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct block {
    size_t size;
    int free;
    struct block *next;
} block;

block *free_list = NULL;

void *myalloc(size_t size);
void *find_free_block(size_t size);
void release_block(void *ptr);
void *current_memory_break();

void *myalloc(size_t size) {

    block *new_block = find_free_block(size);
    if (new_block) {
        new_block->free = 0;
        printf("found free momory\n");
        return (void *)(new_block + 1);
    }

    void *rawmem = sbrk(sizeof(block) + size);
    if (rawmem == (void *)-1) {
        perror("Allocation failed");
    }

    printf("Allocating new one\n");
    new_block = (block *)rawmem;
    new_block->size = size;
    new_block->free = 0;
    new_block->next = free_list;
    free_list = new_block;

    return (void *)(new_block + 1);
}

void *find_free_block(size_t size) {

    block *curr = free_list;
    while (curr) {
        if (curr->free && curr->size >= size) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

void release_block(void *ptr) {
    block *mem = ((block *)ptr) - 1;
    mem->free = 1;
}

void *current_memory_break() {
    void *p = sbrk(0);
    if (p == (void *)-1) {
        perror("error in sbrk while getting current program break");
    }
    return p;
}

int main() {

    printf("Current memory break: %p\n", current_memory_break());
    printf("Size of block: %ld\n", sizeof(block));

    int *user_data = (int *)myalloc(sizeof(int));
    *user_data = 39;

    printf("Value: %d\n", *user_data);
    printf("%ld\n", sizeof(*user_data));

    block *header = ((block *)user_data) - 1;
    printf("%ld\n", header->size);

    release_block(user_data);
    return 0;
}
