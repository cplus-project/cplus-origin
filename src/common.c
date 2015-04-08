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

void error_list_init(error_list* errlist, int8 upper_limit) {
    errlist->err_count   = 0;
    errlist->upper_limit = upper_limit;
    errlist->head        = NULL;
}

// add an error into the error list. if the number of the errors in
// the list will exceed the errlist->upper_limit, the return value
// will be false and the compiler can stop the work and tell the
// programmers to correct errors.
bool error_list_add(error_list* errlist, error err, int32 err_type, int32 line_count) {
    if (errlist->err_count+1 > errlist->upper_limit) {
        return false;
    }
    error_list_node* create = (error_list_node*)mem_alloc(sizeof(error_list_node));
    create->err        = err;
    create->err_type   = err_type;
    create->line_count = line_count;
    create->next       = NULL;
    if (errlist->head != NULL) {
        errlist->tail->next = create;
        errlist->tail = create;
    }
    else {
        errlist->head = create;
        errlist->tail = create;
    }
    errlist->err_count++;
    return true;
}

int8 error_list_err_count(error_list* errlist) {
    return errlist->err_count;
}

void error_list_display(error_list* errlist) {
    int count = 1;
    error_list_node* ptr;
    for (ptr = errlist->head; ptr != NULL; ptr = ptr->next) {
        printf("error %d: %s\r\n", count, ptr->err);
        count++;
    }
}

void error_list_destroy(error_list* errlist) {
    error_list_node* ptr = NULL;
    for (;;) {
        if (errlist->head == NULL) {
            errlist->tail =  NULL;
            return;
        }
        ptr = errlist->head;
        errlist->head = errlist->head->next;
        mem_free(ptr);
    }
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
