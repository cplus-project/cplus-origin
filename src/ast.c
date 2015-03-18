/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "ast.h"

/****** methods of include_list ******/

void include_list_init(include_list* icldlist) {
    icldlist->head = NULL;
    icldlist->tail = NULL;
}

void include_list_add(include_list* icldlist, char* file) {
    include_list_node* create = (include_list_node*)mem_alloc(sizeof(include_list_node));
    create->file = file;
    if (icldlist->head != NULL) {
        icldlist->tail->next = create;
        icldlist->tail       = create;
    }
    else {
        icldlist->head = create;
        icldlist->tail = create;
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
    include_list_node* temp;
    icldlist->tail = icldlist->head;
    for (;;) {
        if (icldlist->tail == NULL) {
            icldlist->head =  NULL;
            return;
        }
        temp = icldlist->tail;
        icldlist->tail = icldlist->tail->next;
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
            printf(" <-tail\r\n");
            return;
        }
        printf("\r\n");
        printf("%s", ptr->file);
        ptr = ptr->next;
    }
}

/****** methods of stmt_block  ******/

void stmt_block_init(stmt_block* block) {
    block->fst = NULL;
    block->lst = NULL;
}

void stmt_block_add(stmt_block* block, ast_node* astnode) {
    stmt_block_entry* create = (stmt_block_entry*)mem_alloc(sizeof(stmt_block_entry));
    create->astnode = astnode;
    create->next    = NULL;
    if (block->fst != NULL) {
        block->lst->next = create;
        block->lst       = create;
    }
    else {
        block->fst = create;
        block->lst = create;
    }
}

void stmt_block_destroy(stmt_block* block) {
    stmt_block_entry* temp;
    block->lst = block->fst;
    for (;;) {
        if (block->lst == NULL) {
            block->fst =  NULL;
            return;
        }
        temp = block->lst;
        block->lst = block->lst->next;
        mem_free(temp);
    }
}
