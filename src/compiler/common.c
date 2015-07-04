/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "common.h"

error new_error(char* errmsg) {
    if (errmsg == NULL) {
        return "";
    }
    return errmsg;
}

void* mem_alloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr != NULL) {
        return ptr;
    }
    assert("malloc panic!!!");
}

void mem_free (void *ptr) {
    if (ptr != NULL) {
        free(ptr);
        ptr = NULL;
    }
}

void debug(char* msg) {
    printf("%s\r\n", msg);
}

void fatal(char* msg) {
    fprintf(stderr, "%s", msg);
    exit(EXIT_FAILURE);
}
