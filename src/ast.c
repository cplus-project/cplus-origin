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

/****** methods of stmt_block ******/

void stmt_block_init(stmt_block* block, stmt_block* outer) {
    scope_init(&block->scp, &outer->scp);
    block->fst = NULL;
}

void stmt_block_destroy(stmt_block* block) {
    scope_destroy(&block->scp);
    block->fst = NULL;
}

/****** methods of ast_node_stack ******/

void ast_node_stack_init(ast_node_stack* stk) {
    stk->top = NULL;
}

void ast_node_stack_push(ast_node_stack* stk, ast_node node) {
    ast_node_stack_node* create = (ast_node_stack_node*)mem_alloc(sizeof(ast_node_stack_node));
    create->node = node;
    create->next = NULL;
    if (stk->top != NULL) {
        create->next = stk->top;
        stk->top = create;
    }
    else {
        stk->top = create;
    }
}

error ast_node_stack_pop(ast_node_stack* stk) {
    if (stk->top == NULL) {
        return new_error("err: the ast node stack is empty.");
    }
    ast_node_stack_node* temp = stk->top;
    stk->top = stk->top->next;
    mem_free(temp);
}

ast_node* ast_node_stack_top(ast_node_stack* stk) {
    if (stk->top != NULL) {
        return &stk->top->node;
    }
    return NULL;
}

void ast_node_stack_destroy(ast_node_stack* stk) {
    ast_node_stack_node* temp = NULL;
    for (;;) {
        if (stk->top == NULL) {
            return;
        }
        temp = stk->top;
        stk->top = stk->top->next;
        mem_free(temp);
    }
}

/****** methods of ast ******/

void ast_init(ast* astree) {
    include_list_init(&astree->include_files);
    id_table_init(&astree->modules);
    astree->fst = (ast_node*)mem_alloc(sizeof(ast_node));
    astree->fst->next        = NULL;
    astree->fst->line_count  = 0;
    astree->fst->line_pos    = 0;
    astree->fst->syntax_type = STMT_BLOCK;
    astree->fst->syntax_entity.syntax_block = (stmt_block*)mem_alloc(sizeof(stmt_block));
    // the astree->fst point to the global block of the parsing
    // file, so set its outer block to NULL.
    stmt_block_init(astree->fst->syntax_entity.syntax_block, NULL);
    astree->cur       = astree->fst;
    astree->cur_block = astree->fst->syntax_entity.syntax_block;
}

void ast_destroy(ast* astree) {
    include_list_destroy(&astree->include_files);
    id_table_destroy(&astree->modules);
}
