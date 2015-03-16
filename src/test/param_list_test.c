/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The test for param_list of src/func.h and
 * src/func.c
 **/

#include "../func.h"

int main() {
    param_list paralst;
    param_list_init(&paralst);
    param_list_add(&paralst, "int32");
    param_list_add(&paralst, "int64");
    param_list_add(&paralst, "int32");
    param_list_add(&paralst, "string");
    param_list_node* ptr = NULL;
    for (ptr = paralst.head; ptr != NULL; ptr = ptr->next) {
        debug(ptr->type);
    }
    param_list_destroy(&paralst);
    return 0;
}
