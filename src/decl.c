/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "decl.h"

void decl_list_init(decl_list* declist) {
    declist->head = NULL;
    declist->tail = NULL;
}

void decl_list_add(decl_list* declist, declare decl) {
    decl_list_node* create = (decl_list_node*)mem_alloc(sizeof(decl_list_node));
    create->decl = decl;
    create->next = NULL;
    if (declist->head != NULL) {
        declist->tail->next = create;
        declist->tail       = create;
    }
    else {
        declist->head = create;
        declist->tail = create;
    }
}

void decl_list_destroy(decl_list* declist) {
    decl_list_node* ptr = declist->head;
    decl_list_node* temp;
    for (;;) {
        if (ptr == NULL) {
            return;
        }
        temp = ptr;
        ptr  = ptr->next;
        mem_free(temp);
    }
    declist->head = NULL;
    declist->tail = NULL;
}

void decl_list_debug(decl_list* declist) {
    decl_list_node* ptr;
    printf("all nodes in the declare list is:\r\n");
    if (declist->head == NULL) {
        printf("none nodes\r\n");
        return;
    }
    printf("column1->type  column2->name  column3->assign\r\n");
    printf("%s %s %s <-top", declist->head->decl.decl_type, declist->head->decl.decl_name, declist->head->decl.decl_assign);
    for (ptr = declist->head->next; ptr != NULL; ptr = ptr->next) {
        printf("\r\n%s %s %s", ptr->decl.decl_type, ptr->decl.decl_name, ptr->decl.decl_assign);
    }
    printf(" <-cur\r\n");
}
