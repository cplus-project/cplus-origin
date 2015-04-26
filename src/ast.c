/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "ast.h"

/****** methods of ast_node_block ******/

void ast_node_block_addstmt(ast_node_block* block, ast_node* node) {
    ast_node_block_node* create = (ast_node_block_node*)mem_alloc(sizeof(ast_node_block_node));
    create->node = node;
    create->next = NULL;
    if (block->head != NULL) {
        block->tail->next = create;
        block->tail = create;
    }
    else {
        block->head = create;
        block->tail = create;
    }
}
