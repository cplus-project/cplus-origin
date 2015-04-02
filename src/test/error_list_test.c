/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The test for error_list in common.h and common.c
 **/

#include "../common.h"

int main() {
    int  i = 0;
    error err[] = {
    "err: file not found.",
    "err: the platform not supported.",
    "err: can not access the file.",
    "err: operation not be permitted."
    };
    error_list errlist;
    error_list_init(&errlist, 3);
    for (;;) {
        if (error_list_add(&errlist, err[i], ERROR_TYPE_GENERAL, 0, 0) == false) {
            break;
        }
        i++;
    }
    error_list_node* ptr;
    for (ptr = errlist.head; ptr != NULL; ptr = ptr->next) {
        printf("the error is : %s\r\n", ptr->err);
    }
    error_list_destroy(&errlist);

    debug("\r\n\r\nrun over~");
    return 0;
}
