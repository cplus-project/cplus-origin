/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "syntax.h"

static error syntax_analyzer_parse_import(syntax_analyzer* syx, ast_elem_import* elem_import);
static error syntax_analyzer_parse_expr  (syntax_analyzer* syx, ast_elem_expr*   elem_expr);
static error syntax_analyzer_parse_block (syntax_analyzer* syx, ast_elem_block*  elem_block);

#define syntax_analyzer_get_token_without_callnext(syx) \
error err = lex_parse_token(&syx->lex);                     \
if (err != NULL) {                                          \
    if (strcmp(err, "EOF") == 0) {                          \
        return NULL;                                        \
    }                                                       \
    else {                                                  \
        if (error_list_add(&syx->err_list,                  \
                           err,                             \
                           ERROR_TYPE_LEXICAL,              \
                           syx->lex.line_count) == false) { \
            error_list_display(&syx->err_list);             \
            exit(EXIT_FAILURE);                             \
        }                                                   \
        return NULL;                                        \
    }                                                       \
}                                                           \
syx->cur_token = lex_read_token(&syx->lex);

#define syntax_analyzer_get_token_with_callnext(syx) \
error err = lex_parse_token(&syx->lex);                     \
if (err != NULL) {                                          \
    if (strcmp(err, "EOF") == 0) {                          \
        return err;                                         \
    }                                                       \
    else {                                                  \
        if (error_list_add(&syx->err_list,                  \
                           err,                             \
                           ERROR_TYPE_LEXICAL,              \
                           syx->lex.line_count) == false) { \
            error_list_display(&syx->err_list);             \
            exit(EXIT_FAILURE);                             \
        }                                                   \
        return NULL;                                        \
    }                                                       \
}                                                           \
syx->cur_token = lex_read_token(&syx->lex);                 \
lex_next_token(&syx->lex);

#define check_error(err) \
if (strcmp(err, "EOF") == 0) { \
    return NULL;               \
}                              \
else {                         \
    return err;                \
}

error syntax_analyzer_init(syntax_analyzer* syx, char* file) {
    error err = lex_init(&syx->lex);
    if (err != NULL) {
        return err;
    }
    err = lex_open_srcfile(&syx->lex, file);
    if (err != NULL) {
        return err;
    }
    syx->cur_token = NULL;
    ast_init(&syx->astree);
    error_list_init(&syx->err_list, 50);
}

static error syntax_analyzer_parse_import(syntax_analyzer* syx, ast_elem_import* elem_import) {
    elem_import = (ast_elem_import*)mem_alloc(sizeof(ast_elem_import));
    elem_import->icldlist = (include_list*)mem_alloc(sizeof(include_list));
    elem_import->modlist  = (module_list*) mem_alloc(sizeof(module_list));
    include_list_init(elem_import->icldlist);
     module_list_init(elem_import->modlist);

    int16           last_token_type;
    dynamicarr_char darr;
    dynamicarr_char_init(&darr, 255);

    for (;;) {
        syntax_analyzer_get_token_without_callnext(syx);
        if (syx->cur_token->token_type == TOKEN_KEYWORD_INCLUDE) {
            lex_next_token(&syx->lex);
            for (;;) {
                syntax_analyzer_get_token_with_callnext(syx);
                switch (syx->cur_token->token_type) {
                case TOKEN_OP_LT:
                    for (;;) {  
                        syntax_analyzer_get_token_with_callnext(syx);
                        if (syx->cur_token->token_type == TOKEN_OP_GT) {
                            include_list_add(elem_import->icldlist, dynamicarr_char_getstr(&darr));
                            dynamicarr_char_clear(&darr);
                        }
                        else {
                            dynamicarr_char_append(&darr, lex_token_getstr(syx->cur_token), syx->cur_token->token_len);
                        }
                    }
                    break;
                case TOKEN_CONST_STRING:
                    include_list_add(elem_import->icldlist, lex_token_getstr(syx->cur_token));
                    break;
                default:
                    break;
                }
            }
        }
        else if (syx->cur_token->token_type == TOKEN_KEYWORD_MODULE) {
            lex_next_token(&syx->lex);
            last_token_type = TOKEN_UNKNOWN;
            for (;;) {
                syntax_analyzer_get_token_without_callnext(syx);
                switch (syx->cur_token->token_type) {
                case TOKEN_ID:
                    if (last_token_type == TOKEN_UNKNOWN || last_token_type == TOKEN_OP_DIV) {
                        dynamicarr_char_append(&darr, lex_token_getstr(syx->cur_token), syx->cur_token->token_len);
                        last_token_type = TOKEN_ID;
                    }
                    else {
                        // TODO: report error...
                    }
                    break;
                case TOKEN_OP_DIV:
                    if (last_token_type == TOKEN_ID) {
                        dynamicarr_char_appendc(&darr, '/');
                        last_token_type = TOKEN_OP_DIV;
                    }
                    else {
                        // TODO: report error...
                    }
                    break;
                case TOKEN_NEXT_LINE:
                    module_list_add(elem_import->modlist, dynamicarr_char_getstr(&darr));
                    break;
                default:
                    // TODO: report error...
                    break;
                }
            }
        }
        else {
            return NULL;
        }
    }
}

static error syntax_analyzer_parse_expr(syntax_analyzer* syx, ast_elem_expr* elem_expr) {
    ast_elem_stack oprd_stk;
    ast_elem_stack optr_stk;
    ast_elem_stack_init(&oprd_stk);
    ast_elem_stack_init(&optr_stk);
    for (;;) {
        syntax_analyzer_get_token_without_callnext(syx);
        if (syx->cur_token->token_type == TOKEN_ID) {
            ast_elem* elem = (ast_elem*)mem_alloc(sizeof(ast_elem));
            elem->ast_elem_type = AST_ELEM_ID;
            elem->ast_elem_entity.elem_id = (ast_elem_id*)mem_alloc(sizeof(ast_elem_if));
            elem->ast_elem_entity.elem_id->line_count = syx->lex.line_count;
            elem->ast_elem_entity.elem_id->line_pos   = 0;
            elem->ast_elem_entity.elem_id->id_name    = lex_token_getstr(syx->cur_token);
            ast_elem_stack_push(&oprd_stk, elem);
        }
        else if (syx->cur_token->token_type == TOKEN_OP_SPOT) {
            // TODO: ...
        }
        else if (TOKEN_CONST_INTEGER <= syx->cur_token->token_type && syx->cur_token->token_type <= TOKEN_CONST_STRING) {
            ast_elem* elem = (ast_elem*)mem_alloc(sizeof(ast_elem));
            switch (syx->cur_token->token_type) {
            case TOKEN_CONST_INTEGER:
                elem->ast_elem_type = AST_ELEM_CONST_INTEGER;
                elem->ast_elem_entity.elem_const_integer = (ast_elem_const_integer*)mem_alloc(sizeof(ast_elem_const_integer));
                elem->ast_elem_entity.elem_const_integer->line_count = syx->lex.line_count;
                elem->ast_elem_entity.elem_const_integer->line_pos   = 0;
                elem->ast_elem_entity.elem_const_integer->value      = lex_token_getstr(syx->cur_token);
                break;
            case TOKEN_CONST_FLOAT:
                elem->ast_elem_type = AST_ELEM_CONST_FLOAT;
                elem->ast_elem_entity.elem_const_float = (ast_elem_const_float*)mem_alloc(sizeof(ast_elem_const_float));
                elem->ast_elem_entity.elem_const_float->line_count = syx->lex.line_count;
                elem->ast_elem_entity.elem_const_float->line_pos   = 0;
                elem->ast_elem_entity.elem_const_float->value      = lex_token_getstr(syx->cur_token);
                break;
            case TOKEN_CONST_CHAR:
                elem->ast_elem_type = AST_ELEM_CONST_CHAR;
                elem->ast_elem_entity.elem_const_char = (ast_elem_const_char*)mem_alloc(sizeof(ast_elem_const_char));
                elem->ast_elem_entity.elem_const_char->line_count = syx->lex.line_count;
                elem->ast_elem_entity.elem_const_char->line_pos   = 0;
                elem->ast_elem_entity.elem_const_char->value      = lex_token_getstr(syx->cur_token)[0];
                break;
            case TOKEN_CONST_STRING:
                elem->ast_elem_type = AST_ELEM_CONST_STRING;
                elem->ast_elem_entity.elem_const_string = (ast_elem_const_string*)mem_alloc(sizeof(ast_elem_const_string));
                elem->ast_elem_entity.elem_const_string->line_count = syx->lex.line_count;
                elem->ast_elem_entity.elem_const_string->line_pos   = 0;
                elem->ast_elem_entity.elem_const_string->value      = lex_token_getstr(syx->cur_token);
                break;
            default:
                // TODO: report error...
                break;
            }
            ast_elem_stack_push(&oprd_stk, elem);
        }
        else if (TOKEN_OP_ADD <= syx->cur_token->token_type && syx->cur_token->token_type <= TOKEN_OP_NEG) {
            ast_elem* elem = (ast_elem*)mem_alloc(sizeof(ast_elem));
            elem->ast_elem_type = AST_ELEM_OP;
            elem->ast_elem_entity.elem_op = (ast_elem_op*)mem_alloc(sizeof(ast_elem_op));
            elem->ast_elem_entity.elem_op->line_count    = syx->lex.line_count;
            elem->ast_elem_entity.elem_op->line_pos      = 0;
            elem->ast_elem_entity.elem_op->op_token_code = syx->cur_token->token_type;
            
            ast_elem* top_op = ast_elem_stack_top(&optr_stk);
            if (top_op != NULL) {
                // TODO: first to check the priority of the two operators, and then decide to do which operation.
            }
            else {
                ast_elem_stack_push(&optr_stk, elem);
            }
        }
        else {
            // TODO: report error...
        }
    }
    ast_elem_stack_destroy(&oprd_stk);
    ast_elem_stack_destroy(&optr_stk);
}

static error syntax_analyzer_parse_block(syntax_analyzer* syx, ast_elem_block* elem_block) {
    elem_block = (ast_elem_block*)mem_alloc(sizeof(ast_elem_block));
}

ast* syntax_analyzer_generate_ast(syntax_analyzer* syx) {
    error err = NULL;
    err = syntax_analyzer_parse_import(syx, syx->astree.imports);
    if (err != NULL || syx->astree.imports == NULL) {
        return NULL;
    }
    err = syntax_analyzer_parse_block (syx, syx->astree.global_block);
    if (err != NULL || syx->astree.global_block == NULL) {
        return NULL;
    }
    return &syx->astree;
}

void syntax_analyzer_destroy(syntax_analyzer* syx) {
    lex_close_srcfile(&syx->lex);
    lex_destroy(&syx->lex);
    syx->cur_token = NULL;
    ast_destroy(&syx->astree);
    error_list_destroy(&syx->err_list);
}

#undef syntax_analyzer_get_token_with_callnext
#undef syntax_analyzer_get_token_without_callnext
#undef check_error
