#include "allocator.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define MIN_PAYLOAD 16
#define ALIGNMENT 16
#define ALIGN(x) (((x) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))
#define HEADER_SIZE ALIGN(sizeof(block))
#define FOOTER_SIZE ALIGN(sizeof(footer))
#define BLOCK_OVERHEAD (HEADER_SIZE + FOOTER_SIZE)
#define MIN_BLOCK_SIZE (BLOCK_OVERHEAD + MIN_PAYLOAD)

typedef struct block {
    size_t size;
    int free;
    struct block *next;
} block;

typedef block footer;

static block *free_list = NULL;

block *find_free_block(size_t size);
void *current_memory_break();
void split_block(block *ptr, size_t size);
void remove_from_free_list(block *b);
void insert_into_free_list(block *b);

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

void *myalloc(size_t size) {
    size_t payload_size = ALIGN(size);
    size_t total_size = payload_size + BLOCK_OVERHEAD;

    block *free_block = find_free_block(total_size);
    if (free_block) {
        remove_from_free_list(free_block);
        if (free_block->size >= total_size + MIN_BLOCK_SIZE) {
            split_block(free_block, payload_size);
        }
        free_block->free = 0;

        footer *f =
            (footer *)((char *)free_block + free_block->size - FOOTER_SIZE);
        f->free = 0;

        return (void *)((char *)free_block + HEADER_SIZE);
    }

    void *rawmem = sbrk(total_size);
    if (rawmem == (void *)-1) {
        errno = ENOMEM;
        return NULL;
    }

    block *new_allocated_block = (block *)rawmem;
    new_allocated_block->size = total_size;
    new_allocated_block->free = 0;

    footer *ftr =
        (footer *)((char *)new_allocated_block + total_size - FOOTER_SIZE);
    ftr->size = total_size;
    ftr->free = 0;
    return (void *)((char *)new_allocated_block + HEADER_SIZE);
}

block *find_free_block(size_t size) {

    block *curr = free_list;
    while (curr) {
        if (curr->free && curr->size >= size) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

void remove_from_free_list(block *b) {
    if (!b || !free_list)
        return;

    if (free_list == b) {
        free_list = b->next;
        return;
    }

    block *curr = free_list;
    while (curr->next && curr->next != b)
        curr = curr->next;

    if (curr->next == b)
        curr->next = b->next;
}

void split_block(block *ptr, size_t requested_payload) {

    if (!ptr || !ptr->free)
        return;

    size_t allocated_size = requested_payload + BLOCK_OVERHEAD;
    size_t remaining_size = ptr->size - allocated_size;

    if (remaining_size < MIN_BLOCK_SIZE) {
        return;
    }

    block *new_block = (block *)((char *)ptr + allocated_size);
    new_block->size = remaining_size;
    new_block->free = 1;

    footer *new_block_footer =
        (footer *)((char *)new_block + remaining_size - FOOTER_SIZE);
    new_block_footer->size = remaining_size;
    new_block_footer->free = 1;

    ptr->size = allocated_size;
    ptr->free = 0;

    footer *ptr_footer = (footer *)((char *)ptr + allocated_size - FOOTER_SIZE);
    ptr_footer->size = allocated_size;
    ptr_footer->free = 0;
    insert_into_free_list(new_block);
}

void insert_into_free_list(block *b) {
    b->next = free_list;
    free_list = b;
}

void release_block(void *ptr) {

    if (!ptr)
        return;

    block *mem = (block *)(((char *)ptr) - HEADER_SIZE);

    if (mem->free)
        abort();

    mem->free = 1;

    footer *ftr = (footer *)((char *)ptr + mem->size - FOOTER_SIZE);

    ftr->free = 1;

    insert_into_free_list(mem);
}

void *current_memory_break() {
    void *p = sbrk(0);
    if (p == (void *)-1) {
        return NULL;
    }
    return p;
}
