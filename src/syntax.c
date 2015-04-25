/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "syntax.h"

static error syntax_analyzer_parse_import       (syntax_analyzer* syx, ast_elem_import*        elem_import);
static error syntax_analyzer_parse_block        (syntax_analyzer* syx, ast_elem_block*         elem_block);
static error syntax_analyzer_parse_derefer      (syntax_analyzer* syx, ast_elem_derefer*       elem_derefer, ast_elem* left, ast_elem_id* right);
static error syntax_analyzer_parse_expr         (syntax_analyzer* syx, ast_elem_expr*          elem_expr);
static error syntax_analyzer_parse_if           (syntax_analyzer* syx, ast_elem_if*            elem_if);
static error syntax_analyzer_parse_ef           (syntax_analyzer* syx, ast_elem_ef*            elem_ef);
static error syntax_analyzer_parse_else         (syntax_analyzer* syx, ast_elem_else*          elem_else);
static error syntax_analyzer_parse_branch_if    (syntax_analyzer* syx, ast_elem_branch_if*     elem_branch_if);
static error syntax_analyzer_parse_loop_for     (syntax_analyzer* syx, ast_elem_loop_for*      elem_loop_for);
static error syntax_analyzer_parse_loop_while   (syntax_analyzer* syx, ast_elem_loop_while*    elem_loop_while);
static error syntax_analyzer_parse_loop_infinite(syntax_analyzer* syx, ast_elem_loop_infinite* elem_loop_infinite);
static error syntax_analyzer_parse_loop_foreach (syntax_analyzer* syx, ast_elem_loop_foreach*  elem_loop_foreach);
static error syntax_analyzer_parse_func_call    (syntax_analyzer* syx, ast_elem_func_call*     elem_func_call, ast_elem* func_name);

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

#define syntax_analyzer_check_err(err) \
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

static error syntax_analyzer_parse_block(syntax_analyzer* syx, ast_elem_block* elem_block) {
    elem_block = (ast_elem_block*)mem_alloc(sizeof(ast_elem_block));
    ast_elem_block_init(elem_block);

    error err = NULL;
    for (;;) {
        syntax_analyzer_get_token_with_callnext(syx);
        switch (syx->cur_token->token_type) {
        case TOKEN_ID: {
                ast_elem* elem = (ast_elem*)mem_alloc(sizeof(ast_elem));
                elem->ast_elem_type = AST_ELEM_ID;
                elem->ast_elem_entity.elem_id = (ast_elem_id*)mem_alloc(sizeof(ast_elem_id));
                elem->ast_elem_entity.elem_id->line_count = syx->lex.line_count;
                elem->ast_elem_entity.elem_id->line_pos   = 0;
                elem->ast_elem_entity.elem_id->id_name    = lex_token_getstr(syx->cur_token);
                ast_elem_block_add(elem_block, elem);
            }
            break;
        case TOKEN_OP_SPOT: {
                syntax_analyzer_get_token_without_callnext(syx);
                if (syx->cur_token->token_type != AST_ELEM_ID) {
                    // TODO: report error...
                }
                lex_next_token(&syx->lex);

                ast_elem_id* right = (ast_elem_id*)mem_alloc(sizeof(ast_elem_id));
                right->line_count = syx->lex.line_count;
                right->line_pos   = 0;
                right->id_name    = lex_token_getstr(syx->cur_token);

                ast_elem* left = ast_elem_block_get_back(elem_block);
                if (left == NULL) {
                    // TODO: report error...
                }

                ast_elem* elem = (ast_elem*)mem_alloc(sizeof(ast_elem));
                elem->ast_elem_type = AST_ELEM_DEREFER;
                err = syntax_analyzer_parse_derefer(syx, elem->ast_elem_entity.elem_derefer, left, right);
                syntax_analyzer_check_err(err);

                ast_elem_block_replace_back(elem_block, elem);
            }
            break;
        case TOKEN_OP_LPARENTHESE: {
                ast_elem* func_name = ast_elem_block_get_back(elem_block);
                if (func_name == NULL) {
                    // TODO: report error...
                }
                ast_elem* elem = (ast_elem*)mem_alloc(sizeof(ast_elem));
                elem->ast_elem_type = AST_ELEM_FUNC_CALL;
                err = syntax_analyzer_parse_func_call(syx, elem->ast_elem_entity.elem_func_call, func_name);
                syntax_analyzer_check_err(err);
                ast_elem_block_replace_back(elem_block, elem);
            }
            break;
        default:
            // TODO: report error...
            break;
        }
    }
}

// note:
//    this function will not release the memory of the left and right.
static error syntax_analyzer_parse_derefer(syntax_analyzer* syx, ast_elem_derefer* elem_derefer, ast_elem* left, ast_elem_id* right) {
    ast_elem_derefer* new_derefer = (ast_elem_derefer*)mem_alloc(sizeof(ast_elem_derefer));
    new_derefer->derefer_right_type = AST_ELEM_ID;
    new_derefer->derefer_right.derefer_right_id = right;

    switch (left->ast_elem_type) {
    case AST_ELEM_ID:
        new_derefer->derefer_left_type = AST_ELEM_ID;
        new_derefer->derefer_left.derefer_left_id = left->ast_elem_entity.elem_id;
        elem_derefer = new_derefer;
        break;
    case AST_ELEM_DEREFER: {
            ast_elem_derefer* last_derefer = left->ast_elem_entity.elem_derefer;
            while (last_derefer->derefer_right_type == AST_ELEM_DEREFER) {
                last_derefer = last_derefer->derefer_right.derefer_right_derefer;
            }
            switch (last_derefer->derefer_right_type) {
            case AST_ELEM_ID:
                new_derefer->derefer_left_type = AST_ELEM_ID;
                new_derefer->derefer_left.derefer_left_id = last_derefer->derefer_right.derefer_right_id;
                break;
            case AST_ELEM_FUNC_CALL:
                new_derefer->derefer_left_type = AST_ELEM_FUNC_CALL;
                new_derefer->derefer_left.derefer_left_func_call = last_derefer->derefer_right.derefer_right_func_call;
                break;
            case AST_ELEM_ARRELEM:
                new_derefer->derefer_left_type = AST_ELEM_ARRELEM;
                new_derefer->derefer_left.derefer_left_arrelem = last_derefer->derefer_right.derefer_right_arrelem;
                break;
            default:
                // TODO: report error...
                break;
            }
            last_derefer->derefer_right_type = AST_ELEM_DEREFER;
            last_derefer->derefer_right.derefer_right_derefer = new_derefer;
            elem_derefer = left->ast_elem_entity.elem_derefer;
        }
        break;
    case AST_ELEM_FUNC_CALL:
        new_derefer->derefer_left_type = AST_ELEM_FUNC_CALL;
        new_derefer->derefer_left.derefer_left_func_call = left->ast_elem_entity.elem_func_call;
        elem_derefer = new_derefer;
        break;
    case AST_ELEM_ARRELEM:
        new_derefer->derefer_left_type = AST_ELEM_ARRELEM;
        new_derefer->derefer_left.derefer_left_arrelem = left->ast_elem_entity.elem_arrelem;
        elem_derefer = new_derefer;
        break;
    default:
        // TODO: report error...
        break;
    }
    return NULL;
}

static error syntax_analyzer_calcu_once_in_exprstk(ast_elem_stack* oprd_stk, ast_elem* optr) {
    ast_elem* calcued_oprd = (ast_elem*)mem_alloc(sizeof(ast_elem));
    // binary operators
    if (TOKEN_OP_ADD <= optr->ast_elem_entity.elem_op->op_token_code && optr->ast_elem_entity.elem_op->op_token_code <= TOKEN_OP_SHR) {
        ast_elem* oprd1 = ast_elem_stack_top(oprd_stk);
        if (oprd1 == NULL) {
            // TODO: report error...
        }
        ast_elem_stack_pop(oprd_stk);
        ast_elem* oprd2 = ast_elem_stack_top(oprd_stk);
        if (oprd2 == NULL) {
            // TODO: report error...
        }
        ast_elem_stack_pop(oprd_stk);
        calcued_oprd->ast_elem_type = AST_ELEM_EXPR_BINARY;
        calcued_oprd->ast_elem_entity.elem_expr_binary = (ast_elem_expr_binary*)mem_alloc(sizeof(ast_elem_expr_binary));
        calcued_oprd->ast_elem_entity.elem_expr_binary->optr_token_code = optr->ast_elem_entity.elem_op->op_token_code;
        calcued_oprd->ast_elem_entity.elem_expr_binary->oprd1           = oprd1;
        calcued_oprd->ast_elem_entity.elem_expr_binary->oprd2           = oprd2;
    }
    // unary operators
    else {
        ast_elem* oprd = ast_elem_stack_top(oprd_stk);
        if (oprd == NULL) {
            // report error...
        }
        ast_elem_stack_pop(oprd_stk);
        calcued_oprd->ast_elem_type = AST_ELEM_EXPR_UNARY;
        calcued_oprd->ast_elem_entity.elem_expr_unary = (ast_elem_expr_unary*)mem_alloc(sizeof(ast_elem_expr_unary));
        calcued_oprd->ast_elem_entity.elem_expr_unary->optr_token_code = optr->ast_elem_entity.elem_op->op_token_code;
        calcued_oprd->ast_elem_entity.elem_expr_unary->oprd            = oprd;
    }
    ast_elem_stack_push(oprd_stk, calcued_oprd);
    return NULL;
}

static error syntax_analyzer_parse_expr(syntax_analyzer* syx, ast_elem_expr* elem_expr) {
    elem_expr = (ast_elem_expr*)mem_alloc(sizeof(ast_elem_expr));

    error             err          = NULL;
    ast_elem_stack    oprd_stk;
    ast_elem_stack    optr_stk;
    ast_elem*         top_oprd     = NULL;
    ast_elem*         top_optr     = NULL;
    ast_elem_derefer* last_derefer = NULL;

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
            lex_next_token(&syx->lex);
            syntax_analyzer_get_token_without_callnext(syx);
            if (syx->cur_token->token_type != TOKEN_ID) {
                // TODO: report error...
            }
            lex_next_token(&syx->lex);

            ast_elem_id* right = (ast_elem_id*)mem_alloc(sizeof(ast_elem_id));
            right->line_count = syx->lex.line_count;
            right->line_pos   = 0;
            right->id_name    = lex_token_getstr(syx->cur_token);

            top_oprd = ast_elem_stack_top(&oprd_stk);
            if (top_oprd == NULL) {
                // TODO: report error...
            }
            ast_elem_stack_pop(&oprd_stk);

            ast_elem* elem = (ast_elem*)mem_alloc(sizeof(ast_elem));
            elem->ast_elem_type = AST_ELEM_DEREFER;
            err = syntax_analyzer_parse_derefer(syx, elem->ast_elem_entity.elem_derefer, top_oprd, right);

            ast_elem_stack_push(&oprd_stk, elem);
        }
        else if (syx->cur_token->token_type == TOKEN_OP_LPARENTHESE) {
            ast_elem* elem = (ast_elem*)mem_alloc(sizeof(ast_elem));
            top_oprd = ast_elem_stack_top(&oprd_stk);
            if (top_oprd != NULL) {
                ast_elem_stack_pop(&oprd_stk);
                elem->ast_elem_type = AST_ELEM_FUNC_CALL;
                err = syntax_analyzer_parse_func_call(syx, elem->ast_elem_entity.elem_func_call, top_oprd);
                syntax_analyzer_check_err(err);
                mem_free(top_oprd);
                lex_next_token(&syx->lex);
                ast_elem_stack_push(&oprd_stk, elem);
            }
            else {
                elem->ast_elem_type = AST_ELEM_OP;
                elem->ast_elem_entity.elem_op = (ast_elem_op*)mem_alloc(sizeof(ast_elem_op));
                elem->ast_elem_entity.elem_op->line_count    = syx->lex.line_count;
                elem->ast_elem_entity.elem_op->line_pos      = 0;
                elem->ast_elem_entity.elem_op->op_token_code = TOKEN_OP_LPARENTHESE;
                ast_elem_stack_push(&optr_stk, elem);
            }
        }
        else if (syx->cur_token->token_type == TOKEN_OP_RPARENTHESE || syx->cur_token->token_type == TOKEN_NEXT_LINE) {
            for (;;) {
                top_optr = ast_elem_stack_top(&optr_stk);
                if (top_optr == NULL) {
                    return NULL; // parsing the expression successfully if get here
                }
                ast_elem_stack_pop(&optr_stk);
                if (top_optr->ast_elem_entity.elem_op->op_token_code == TOKEN_OP_LPARENTHESE) {
                    break; // parsing a pair of parenthese('(' and ')') successfully if get here
                }
                err = syntax_analyzer_calcu_once_in_exprstk(&oprd_stk, top_optr);
                if (err != NULL) {
                    // TODO: report error...
                }
                mem_free(top_optr);
            }
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
        else if (TOKEN_OP_ADD <= syx->cur_token->token_type && syx->cur_token->token_type <= TOKEN_OP_SUB) {
            ast_elem* elem = (ast_elem*)mem_alloc(sizeof(ast_elem));
            elem->ast_elem_type = AST_ELEM_OP;
            elem->ast_elem_entity.elem_op = (ast_elem_op*)mem_alloc(sizeof(ast_elem_op));
            elem->ast_elem_entity.elem_op->line_count    = syx->lex.line_count;
            elem->ast_elem_entity.elem_op->line_pos      = 0;
            elem->ast_elem_entity.elem_op->op_token_code = syx->cur_token->token_type;
            ast_elem_op_get_priority(elem->ast_elem_entity.elem_op);
            if (elem->ast_elem_entity.elem_op->op_priority == OP_PRIORITY_NULL) {
                // TODO: report error...
            }

            top_optr = ast_elem_stack_top(&optr_stk);
            if (top_optr == NULL || (elem->ast_elem_entity.elem_op->op_priority > top_optr->ast_elem_entity.elem_op->op_priority)) {
                ast_elem_stack_push(&optr_stk, elem);
            }
            else {
                err = syntax_analyzer_calcu_once_in_exprstk(&oprd_stk, top_optr);
                if (err != NULL) {
                    // TODO: report error...
                }
            }
        }
        else {
            // TODO: report error...
        }
    }
    ast_elem_stack_destroy(&oprd_stk);
    ast_elem_stack_destroy(&optr_stk);
}

error syntax_analyzer_parse_if(syntax_analyzer* syx, ast_elem_if* elem_if) {
    elem_if = (ast_elem_if*)mem_alloc(sizeof(ast_elem_if));
    error err = syntax_analyzer_parse_expr(syx, elem_if->if_cond);
    syntax_analyzer_check_err(err);
    err = syntax_analyzer_parse_block(syx, elem_if->if_block);
    syntax_analyzer_check_err(err);
    return NULL;
}

error syntax_analyzer_parse_ef(syntax_analyzer* syx, ast_elem_ef* elem_ef) {
    elem_ef = (ast_elem_ef*)mem_alloc(sizeof(ast_elem_ef));
    error err = syntax_analyzer_parse_expr(syx, elem_ef->ef_cond);
    syntax_analyzer_check_err(err);
    err = syntax_analyzer_parse_block(syx, elem_ef->ef_block);
    syntax_analyzer_check_err(err);
    return NULL;
}

error syntax_analyzer_parse_else(syntax_analyzer* syx, ast_elem_else* elem_else){
    elem_else = (ast_elem_else*)mem_alloc(sizeof(ast_elem_else));
    error err = syntax_analyzer_parse_block(syx, elem_else->else_block);
    syntax_analyzer_check_err(err);
    return NULL;
}

error syntax_analyzer_parse_branch_if(syntax_analyzer* syx, ast_elem_branch_if* elem_branch_if) {
    elem_branch_if = (ast_elem_branch_if*)mem_alloc(sizeof(ast_elem_branch_if));
    // TODO: ...
    return NULL;
}

error syntax_analyzer_parse_loop_for(syntax_analyzer* syx, ast_elem_loop_for* elem_loop_for) {
    elem_loop_for = (ast_elem_loop_for*)mem_alloc(sizeof(ast_elem_loop_for));
    return NULL;
}

error syntax_analyzer_parse_loop_while(syntax_analyzer* syx, ast_elem_loop_while* elem_loop_while) {
    elem_loop_while = (ast_elem_loop_while*)mem_alloc(sizeof(ast_elem_loop_while));
    error err = syntax_analyzer_parse_expr(syx, elem_loop_while->loop_while_cond);
    syntax_analyzer_check_err(err);
    err = syntax_analyzer_parse_block(syx, elem_loop_while->loop_while_block);
    syntax_analyzer_check_err(err);
    return NULL;
}

error syntax_analyzer_parse_loop_infinite(syntax_analyzer* syx, ast_elem_loop_infinite* elem_loop_infinite) {
    elem_loop_infinite = (ast_elem_loop_infinite*)mem_alloc(sizeof(ast_elem_loop_infinite));
    error err = syntax_analyzer_parse_block(syx, elem_loop_infinite->loop_infinite_block);
    syntax_analyzer_check_err(err);
    return NULL;
}

error syntax_analyzer_parse_loop_foreach(syntax_analyzer* syx, ast_elem_loop_foreach* elem_loop_foreach) {
    elem_loop_foreach = (ast_elem_loop_foreach*)mem_alloc(sizeof(ast_elem_loop_foreach));
    return NULL;
}

static error syntax_analyzer_parse_func_call(syntax_analyzer* syx, ast_elem_func_call* elem_func_call, ast_elem* func_name) {
    elem_func_call = (ast_elem_func_call*)mem_alloc(sizeof(ast_elem_func_call));
    elem_func_call->func_call_parameters = (actual_param_list*)mem_alloc(sizeof(actual_param_list));

    error     err        = NULL;
    ast_elem* last_param = NULL;

    // parsing the function name
    switch (func_name->ast_elem_type) {
    case AST_ELEM_ID:
        elem_func_call->func_call_name = func_name->ast_elem_entity.elem_id->id_name;
        break;
    case AST_ELEM_DEREFER: {
            ast_elem_derefer* last_derefer = func_name->ast_elem_entity.elem_derefer;
            while (last_derefer->derefer_right_type == AST_ELEM_DEREFER) {
                last_derefer = last_derefer->derefer_right.derefer_right_derefer;
            }
            if (last_derefer->derefer_right_type != AST_ELEM_ID) {
                // TODO: report error...
            }
            elem_func_call->func_call_name = last_derefer->derefer_right.derefer_right_id->id_name;
        }
        break;
    default:
        // TODO: report error...
        break;
    }

    // parsing the function's parameters
    for (;;) {
        syntax_analyzer_get_token_without_callnext(syx);
        switch (syx->cur_token->token_type) {
        case TOKEN_CONST_INTEGER: {
                ast_elem* param = (ast_elem*)mem_alloc(sizeof(ast_elem));
                param->ast_elem_type = AST_ELEM_CONST_INTEGER;
                param->ast_elem_entity.elem_const_integer = (ast_elem_const_integer*)mem_alloc(sizeof(ast_elem_const_integer));
                param->ast_elem_entity.elem_const_integer->line_count = syx->lex.line_count;
                param->ast_elem_entity.elem_const_integer->line_pos   = 0;
                param->ast_elem_entity.elem_const_integer->value      = lex_token_getstr(syx->cur_token);
                actual_param_list_add(elem_func_call->func_call_parameters, param);
                lex_next_token(&syx->lex);
            }
            break;
        case TOKEN_ID: {
                ast_elem* param = (ast_elem*)mem_alloc(sizeof(ast_elem));
                param->ast_elem_type = AST_ELEM_ID;
                param->ast_elem_entity.elem_id = (ast_elem_id*)mem_alloc(sizeof(ast_elem_id));
                param->ast_elem_entity.elem_id->line_count = syx->lex.line_count;
                param->ast_elem_entity.elem_id->line_pos   = 0;
                param->ast_elem_entity.elem_id->id_name    = lex_token_getstr(syx->cur_token);
                actual_param_list_add(elem_func_call->func_call_parameters, param);
                lex_next_token(&syx->lex);
            }
            break;
        case TOKEN_CONST_STRING: {
                ast_elem* param = (ast_elem*)mem_alloc(sizeof(ast_elem));
                param->ast_elem_type = AST_ELEM_CONST_STRING;
                param->ast_elem_entity.elem_const_string = (ast_elem_const_string*)mem_alloc(sizeof(ast_elem_const_string));
                param->ast_elem_entity.elem_const_string->line_count = syx->lex.line_count;
                param->ast_elem_entity.elem_const_string->line_pos   = 0;
                param->ast_elem_entity.elem_const_string->value      = lex_token_getstr(syx->cur_token);
                actual_param_list_add(elem_func_call->func_call_parameters, param);
                lex_next_token(&syx->lex);
            }
            break;
        case TOKEN_OP_COMMA:
            break;
        case TOKEN_OP_LPARENTHESE:
            lex_next_token(&syx->lex);
            if (elem_func_call->func_call_parameters->tail == NULL) {
                // TODO: report error...
            }
            last_param = elem_func_call->func_call_parameters->tail->param;
            ast_elem*
            break;
        case TOKEN_OP_SPOT:
            lex_next_token(&syx->lex);
            syntax_analyzer_get_token_without_callnext(syx);
            if (syx->cur_token->token_type != TOKEN_ID) {
                // TODO: report error...
            }
            lex_next_token(&syx->lex);

            ast_elem_id* right = (ast_elem_id*)mem_alloc(sizeof(ast_elem_id));
            right->line_count = syx->lex.line_count;
            right->line_pos   = 0;
            right->id_name    = lex_token_getstr(syx->cur_token);

            if (elem_func_call->func_call_parameters->tail == NULL) {
                // TODO: report error...
            }
            last_param = func_call_parameters->tail->param;

            ast_elem* derefer = (ast_elem*)mem_alloc(sizeof(ast_elem));
            derefer->ast_elem_type = AST_ELEM_DEREFER;
            err = syntax_analyzer_parse_derefer(syx, derefer, last_param, right);
            syntax_analyzer_check_err(err);

            elem_func_call->func_call_parameters->tail->param = derefer;
            break;
        case TOKEN_CONST_FLOAT: {
                ast_elem* param = (ast_elem*)mem_alloc(sizeof(ast_elem));
                param->ast_elem_type = AST_ELEM_CONST_FLOAT;
                param->ast_elem_entity.elem_const_float = (ast_elem_const_float*)mem_alloc(sizeof(ast_elem_const_float));
                param->ast_elem_entity.elem_const_float->line_count = syx->lex.line_count;
                param->ast_elem_entity.elem_const_float->line_pos   = 0;
                param->ast_elem_entity.elem_const_float->value      = lex_token_getstr(syx->cur_token);
                actual_param_list_add(elem_func_call->func_call_parameters, param);
                lex_next_token(&syx->lex);
            }
            break;
        case TOKEN_CONST_CHAR: {
                ast_elem* param = (ast_elem*)mem_alloc(sizeof(ast_elem));
                param->ast_elem_type = AST_ELEM_CONST_CHAR;
                param->ast_elem_entity.elem_const_char = (ast_elem_const_char*)mem_alloc(sizeof(ast_elem_const_char));
                param->ast_elem_entity.elem_const_char->line_count = syx->lex.line_count;
                param->ast_elem_entity.elem_const_char->line_pos   = 0;
                param->ast_elem_entity.elem_const_char->value      = lex_token_getstr(syx->cur_token)[0];
                actual_param_list_add(elem_func_call->func_call_parameters, param);
                lex_next_token(&syx->lex);
            }
            break;
        case TOKEN_OP_RPARENTHESE:
            break;
        default:
            //TODO: report error...
            break;
        }
    }
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
#undef syntax_analyzer_check_error
