#include "allocator.h"
#include <stdio.h>
#include <assert.h>

int main() {
    int *x = myalloc(sizeof(int));
    assert(x != NULL);
    *x = 42;
    assert(*x == 42);

    release_block(x);

    int *y = myalloc(sizeof(int));
    assert(y != NULL);

    printf("test_basic: OK\n");
    return 0;
}
