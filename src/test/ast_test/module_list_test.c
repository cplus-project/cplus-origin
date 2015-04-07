/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The test for module_list in src/ast.h and
 * src/ash.c
 **/

#include "../../ast.h"

int main() {
    int i;
    char* modules[] = {
        "fmt",
        "net",
        "net/http",
        "fmt",
        "crypto/md5",
        "crypto/md5",
        "os"
    };

    module_list modlist;
    module_list_init(&modlist);
    for (i = 0; i < 7; i++) {
        if (module_list_exist(&modlist, modules[i]) == true) {
            printf("module %s has been already imported\r\n", modules[i]);
        }
        else {
            module_list_add(&modlist, modules[i]);
            printf("include %s\r\n", modules[i]);
        }
    }
    printf("\r\n\r\n");
    debug("The Node Of The List:");
    module_list_node* ptr;
    for (ptr = modlist.head; ptr != NULL; ptr = ptr->next) {
        printf("%s\r\n", ptr->module);
    }
    module_list_destroy(&modlist);
    debug("\r\nrun over~");

    return 0;
}
