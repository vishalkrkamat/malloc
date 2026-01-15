#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define MIN_PAYLOAD 16
#define ALIGNMENT 16
#define ALIGN(x) (((x) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))
#define HEADER_SIZE ALIGN(sizeof(block))

typedef struct block {
    size_t payload_size;
    int free;
    struct block *next;
} block;

static block *block_list = NULL;

void test_split(void);
void *myalloc(size_t size);
block *find_free_block(size_t size);
void release_block(void *ptr);
void *current_memory_break();
void split_block(block *ptr, size_t size);

int main() {

    printf("Current memory break: %p\n", current_memory_break());
    printf("Size of block: %ld\n", sizeof(block));

    double *user_data = (double *)myalloc(sizeof(double));
    *user_data = 39;

    printf("Value: %f\n", *user_data);
    printf("%ld\n", sizeof(*user_data));

    block *header = (block *)(((char *)user_data) - HEADER_SIZE);
    printf("%ld\n", header->payload_size);

    printf("Current memory break: %p\n", current_memory_break());
    release_block(user_data);
    test_split();
    return 0;
}

void *myalloc(size_t size) {
    size_t payload_size = ALIGN(size);

    block *free_block = find_free_block(payload_size);
    if (free_block) {
        if (free_block->payload_size >=
            payload_size + HEADER_SIZE + MIN_PAYLOAD) {
            split_block(free_block, payload_size);
        }
        free_block->free = 0;
        printf("found free momory\n");
        return (void *)((char *)free_block + HEADER_SIZE);
    }

    void *rawmem = sbrk(HEADER_SIZE + payload_size);
    if (rawmem == (void *)-1) {
        errno = ENOMEM;
        return NULL;
    }

    printf("Allocating new one\n");
    block *new_allocated_block = (block *)rawmem;
    new_allocated_block->payload_size = payload_size;
    new_allocated_block->free = 0;
    new_allocated_block->next = block_list;
    block_list = new_allocated_block;

    return (void *)((char *)new_allocated_block + HEADER_SIZE);
}

block *find_free_block(size_t size) {

    block *curr = block_list;
    while (curr) {
        if (curr->free && curr->payload_size >= size) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

void split_block(block *ptr, size_t requested_payload) {

    if (!ptr || !ptr->free)
        return;

    block *new_block = (block *)((char *)ptr + HEADER_SIZE + requested_payload);
    new_block->payload_size =
        ptr->payload_size - HEADER_SIZE - requested_payload;
    new_block->free = 1;
    new_block->next = ptr->next;

    ptr->payload_size = requested_payload;
    ptr->next = new_block;
}

void release_block(void *ptr) {

    if (!ptr)
        return;

    block *mem = (block *)(((char *)ptr) - HEADER_SIZE);

    if (mem->free)
        abort();

    mem->free = 1;
}

void *current_memory_break() {
    void *p = sbrk(0);
    if (p == (void *)-1) {
        return NULL;
    }
    return p;
}

void test_split(void) {
    void *p1 = myalloc(128);
    release_block(p1);

    void *brk_before = current_memory_break();
    void *p2 = myalloc(32);
    void *brk_after = current_memory_break();

    block *b1 = (block *)((char *)p2 - HEADER_SIZE);
    block *b2 = b1->next;

    printf("=== SPLIT TEST ===\n");
    printf("b1 payload: %zu\n", b1->payload_size);
    printf("b2 payload: %zu\n", b2->payload_size);
    printf("brk changed: %d\n", brk_before != brk_after);
}
