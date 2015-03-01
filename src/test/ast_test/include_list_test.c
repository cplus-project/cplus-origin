/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The test for include_list in src/ast.h and
 * src/ash.c
 **/

#include "../../ast.h"

int main() {
    int i;
    char* files[6] = {
        "stdio.h",
        "string.h",
        "stdio.h",
        "time.h",
        "malloc.h",
        "malloc.h"
    };

    include_list icldlist;
    include_list_init(&icldlist);
    for (i = 0; i < 6; i++) {
        if (include_list_exist(&icldlist, files[i]) == true) {
            printf("file %s has been already included\r\n", files[i]);
        }
        else {
            include_list_add(&icldlist, files[i]);
            printf("include %s\r\n", files[i]);
        }
    }
    printf("\r\n\r\n");
    include_list_debug(&icldlist);
    include_list_destroy(&icldlist);
    debug("\r\nrun over~");
    return 0;
}
