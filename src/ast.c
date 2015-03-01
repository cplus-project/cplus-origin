/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "ast.h"

/****** methods of include_list ******/

void include_list_init(include_list* icldlist) {
    icldlist->head = NULL;
    icldlist->cur  = NULL;
}

void include_list_add(include_list* icldlist, char* file) {
    include_list_node* create = (include_list_node*)mem_alloc(sizeof(include_list_node));
    create->file = file;
    if (icldlist->head != NULL) {
        icldlist->cur->next = create;
        icldlist->cur       = create;
    }
    else {
        icldlist->head = create;
        icldlist->cur  = create;
    }
}

// check whether the file included is already in the list.
// return true if in the list.
// return false if not in the list.
bool include_list_exist(include_list* icldlist, char* file) {
    int i;
    int len_para = strlen(file);
    int len_comp;
    include_list_node* ptr;
    for (ptr = icldlist->head; ptr != NULL; ptr = ptr->next) {
        len_comp = strlen(ptr->file);
        if (len_para == len_comp) {
            for (i = 0; i <= len_para; i++) {
                if (i == len_para) {
                    return true;
                }
                if (file[i] != ptr->file[i]) {
                    break;
                }
            }
        }
    }
    return false;
}

void include_list_destroy(include_list* icldlist) {
    include_list_node* ptr = icldlist->head;
    include_list_node* temp;
    for (;;) {
        if (ptr == NULL) {
            icldlist->head = NULL;
            icldlist->cur  = NULL;
            return;
        }
        temp = ptr;
        ptr  = ptr->next;
        mem_free(temp);
    }
}

void include_list_debug(include_list* icldlist) {
    printf("all nodes of the include list:\r\n");
    if (icldlist->head == NULL) {
        printf("none nodes\r\n");
        return;
    }
    printf("%s <-head", icldlist->head->file);
    include_list_node* ptr = icldlist->head->next;
    for (;;) {
        if (ptr == NULL) {
            printf(" <-cur\r\n");
            return;
        }
        printf("\r\n");
        printf("%s", ptr->file);
        ptr = ptr->next;
    }
}
