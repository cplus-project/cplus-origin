/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "syntax.h"

/****** methods of token_buffer ******/

void token_buffer_init(token_buffer* tknbuff) {
    tknbuff->top = NULL;
}

// return true is the stack is empty.
bool token_buffer_isempty(token_buffer* tknbuff) {
    if (tknbuff->top == NULL) {
        return true;
    }
    return false;
}

void token_buffer_push(token_buffer* tknbuff, lex_token tkninfo) {
    token_buffer_node* create = (token_buffer_node*)mem_alloc(sizeof(token_buffer_node));
    create->tkninfo = tkninfo;
    create->next    = NULL;
    if (tknbuff->top != NULL) {
        create->next = tknbuff->top;
        tknbuff->top = create;
    } else {
        tknbuff->top = create;
    }
}

lex_token* token_buffer_top(token_buffer* tknbuff) {
    return &tknbuff->top->tkninfo;
}

error token_buffer_pop(token_buffer* tknbuff) {
    if (tknbuff->top != NULL) {
        token_buffer_node* temp = tknbuff->top;
        tknbuff->top = tknbuff->top->next;
        mem_free(temp);
        return NULL;
    }
    else {
        return new_error("err: the token buffer is empty.");
    }
}

void token_buffer_destroy(token_buffer* tknbuff) {
    token_buffer_node* temp;
    for (;;) {
        if (tknbuff->top == NULL) {
            return;
        }
        temp = tknbuff->top;
        tknbuff->top = tknbuff->top->next;
        mem_free(temp);
    }
}

/****** methods of syntax_analyzer ******/

void syntax_analyzer_init(syntax_analyzer* syx, char* file) {
    lex_init(&syx->lex);
    token_buffer_init(&syx->tkn_buff);
    // TODO: initialize the syx->astree
}

error syntax_analyzer_generate_ast(syntax_analyzer* syx) {
    return NULL;
}

void syntax_analyzer_destroy(syntax_analyzer* syx) {
    lex_destroy(&syx->lex);
    token_buffer_destroy(&syx->tkn_buff);
    // TODO; destroy the syx->astree
}
