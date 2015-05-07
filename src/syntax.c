/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "syntax.h"

/****** methods of operator_stack ******/

void operator_stack_init(operator_stack* optrstk) {
    optrstk->top = NULL;
}

void operator_stack_push(operator_stack* optrstk, int16 op_token_code) {
    operator_stack_node* create = (operator_stack_node*)mem_alloc(sizeof(operator_stack_node));
    create->op_token_code = op_token_code;
    create->next          = NULL;
    if (optrstk->top != NULL) {
        create->next = optrstk->top;
        optrstk->top = create;
    }
    else {
        optrstk->top = create;
    }
}

// return true if the stack is empty.
bool operator_stack_isempty(operator_stack* optrstk) {
    if (optrstk->top == NULL) {
        return true;
    }
    return false;
}

int16 operator_stack_top(operator_stack* optrstk) {
    return optrstk->top->op_token_code;
}

void operator_stack_pop(operator_stack* optrstk) {
    operator_stack_node* temp = optrstk->top;
    optrstk->top = optrstk->top->next;
    mem_free(temp);
}

void operator_stack_destroy(operator_stack* optrstk) {
    operator_stack_node* temp;
    for (;;) {
        if (optrstk->top == NULL) {
            return;
        }
        temp = optrstk->top;
        optrstk->top = optrstk->top->next;
        mem_free(temp);
    }
}

/****** methods of syntax_analyzer ******/

#define syntax_analyzer_get_token(syx) \
err = lex_parse_token(syx->lex);           \
if (err != NULL) {                         \
    if ((int64)err == LEX_ERROR_EOF) {     \
        return NULL;                       \
    }                                      \
    return err;                            \
}                                          \
syx->cur_token = lex_read_token(syx->lex);

static error syntax_analyzer_parse_include      (syntax_analyzer* syx);
static error syntax_analyzer_parse_module       (syntax_analyzer* syx);
static error syntax_analyzer_parse_ident_related(syntax_analyzer* syx);
static error syntax_analyzer_parse_idex         (syntax_analyzer* syx);
static error syntax_analyzer_parse_decl         (syntax_analyzer* syx);
static error syntax_analyzer_parse_assign       (syntax_analyzer* syx);
static error syntax_analyzer_parse_branch_if    (syntax_analyzer* syx);
static error syntax_analyzer_parse_branch_switch(syntax_analyzer* syx);
static error syntax_analyzer_parse_loop_for     (syntax_analyzer* syx);
static error syntax_analyzer_parse_loop_while   (syntax_analyzer* syx);
static error syntax_analyzer_parse_loop_infinite(syntax_analyzer* syx);
static error syntax_analyzer_parse_loop_foreach (syntax_analyzer* syx);
static error syntax_analyzer_parse_func_def     (syntax_analyzer* syx);
static error syntax_analyzer_parse_func_call    (syntax_analyzer* syx);
static error syntax_analyzer_parse_return       (syntax_analyzer* syx);
static error syntax_analyzer_parse_type_decl    (syntax_analyzer* syx);
static error syntax_analyzer_parse_type_assign  (syntax_analyzer* syx);
static error syntax_analyzer_parse_type_def     (syntax_analyzer* syx);
static error syntax_analyzer_parse_new          (syntax_analyzer* syx);

void syntax_analyzer_init(syntax_analyzer* syx, char* file_name) {
    file_stack_init(&syx->file_wait_compiled);
    file_stack_push(&syx->file_wait_compiled, file_name);
    file_tree_init(&syx->file_have_compiled);
    syx->cur_token = NULL;
    syx->lex       = NULL;
}

// parsing include statement set of the source file.
static error syntax_analyzer_parse_include(syntax_analyzer* syx) {
    error err = NULL;
    for (;;) {
        syntax_analyzer_get_token(syx);
        if (syx->cur_token->token_type != TOKEN_KEYWORD_INCLUDE) {
            return NULL;
        }
        lex_next_token(syx->lex);
        for (;;) {
            syntax_analyzer_get_token(syx);
            switch (syx->cur_token->token_type) {
            case TOKEN_CONST_STRING:
                break;
            case TOKEN_OP_LT:
                break;
            default:
                // TODO: report error...
                break;
            }
        }
    }
}

// parsing module statement set of the source file.
static error syntax_analyzer_parse_module(syntax_analyzer* syx) {
    error           err       = NULL;
    int16           last_type = TOKEN_UNKNOWN;
    dynamicarr_char darr;
    dynamicarr_char_init(&darr, 255);
    
    for (;;) {
        syntax_analyzer_get_token(syx);
        if (syx->cur_token->token_type != TOKEN_KEYWORD_MODULE) {
            return NULL;
        }
        lex_next_token(syx->lex);
        for (;;) {
            syntax_analyzer_get_token(syx);
            switch (syx->cur_token->token_type) {
            case TOKEN_ID:
                if (last_type == TOKEN_UNKNOWN || last_type == TOKEN_OP_DIV) {
                    dynamicarr_char_append(&darr, lex_token_getstr(syx->cur_token), syx->cur_token->token_len);
                    lex_next_token(syx->lex);
                    last_type = TOKEN_ID;
                }
                else {
                    // TODO: report error...
                }
                break;
            case TOKEN_OP_DIV:
                if (last_type == TOKEN_ID) {
                    dynamicarr_char_append(&darr, lex_token_getstr(syx->cur_token), syx->cur_token->token_len);
                    lex_next_token(syx->lex);
                    last_type = TOKEN_OP_DIV;
                }
                else {
                    // TODO: report error...
                }
                break;
            case TOKEN_NEXT_LINE:
                // TODO: parse the module
                last_type = TOKEN_UNKNOWN;
                break;
            default:
                // TODO: report error...
                break;
            }
        }
    }
}

error syntax_analyzer_work(syntax_analyzer* syx) {
    error err = NULL;
    
    while (file_stack_isempty(&syx->file_wait_compiled) == false) {
        char* file_name = file_stack_top(&syx->file_wait_compiled);
        
        // if the file has been compiled then import the data from the obj file.
        if (file_tree_exist(&syx->file_have_compiled, file_name) == true) {
            // TODO: 
        }
        // if the file has not been compiled then try to compile the file.
        else {
            lex_analyzer lex;
            lex_init(&lex);
            // try to open the source file. the compile operation process will
            // terminate if the open operation failed.
            err = lex_open_srcfile(&lex, file_name);
            if (err != NULL) {
                printf("%s\r\n", err);
                exit(EXIT_FAILURE);
            }
            
            syx->lex = &lex;
            // preprocess the import portion(include and module) of the source file.
            // if all include files or modules have been compiled already, the syntax
            // analyzer will continue to parse the file. if there are some dependences
            // have not been solved, the analyzer will parse the dependences firstly.
            if ((err = syntax_analyzer_parse_include(syx)) != NULL) {
                if ((int64)err == SYNTAX_ERROR_DEPENDENCE_NEEDED) {
                    // TODO: means some dependence files have not been compiled yet...
                }
                else {
                    // TODO: report error...
                }
            }
            if ((err = syntax_analyzer_parse_module(syx)) != NULL) {
                if ((int64)err == SYNTAX_ERROR_DEPENDENCE_NEEDED) {
                    // TODO: means some dependence files have not been compiled yet...
                }
                else {
                    // TODO: report error...
                }
            }
            
            // TODO: don't forget to call file_stack_pop() when the parsing is over.
            
            lex_destroy(&lex);
        }
    }
    
    return NULL;
}

void syntax_analyzer_destroy(syntax_analyzer* syx) {
    file_stack_destroy(&syx->file_wait_compiled);
    file_tree_destroy(&syx->file_have_compiled);
    syx->cur_token = NULL;
    if (syx->lex != NULL) {
        lex_destroy(syx->lex);
    }
    else {
        syx->lex = NULL;
    }
}

#undef syntax_analyzer_get_token