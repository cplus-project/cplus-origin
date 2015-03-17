/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The test for stmt_block in src/ast.h and
 * src/ash.c
 **/

#include "../../ast.h"

int main() {
    int i;
    ast_node n[5];
    for (i = 0; i < 5; i++) {
        n[i].line_count  = i+1;
        n[i].line_pos    = 0;
        n[i].syntax_type = 0;
    }
    stmt_block block;
    stmt_block_init(&block);
    for (i = 0; i < 5; i++) {
        stmt_block_add(&block, &n[i]);
    }
    stmt_block_entry* ptr;
    printf("the correct output should be 1~5 every line:\r\n");
    for (ptr = block.fst; ptr != NULL; ptr = ptr->next) {
        printf("%d\r\n", ptr->astnode->line_count);
    }
    stmt_block_destroy(&block);
    debug("\r\n\r\nrun over~");
    return 0;
}
