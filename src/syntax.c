/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "syntax.h"

#define syntax_analyzer_get_token(syx) \
err = lex_parse_token(syx->lex);            \
if (err != NULL) {                          \
    if (ERROR_CODE(err) == LEX_ERROR_EOF) { \
        return NULL;                        \
    }                                       \
    return err;                             \
}                                           \
syx->cur_token = lex_read_token(syx->lex);

// return NULL if peeking the next token failed.
static lex_token* syntax_analyzer_peek_token(syntax_analyzer* syx) {
    error err = lex_parse_token(syx->lex);
    if (err != NULL) {
        return NULL;
    }
    return lex_read_token(syx->lex);
}

void syntax_analyzer_init(syntax_analyzer* syx, char* file_name) {
    close_counter_init(&syx->clsctr);
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

// parse the expression of the C+. the output will be assigned to the parameter 'expr'.
static error syntax_analyzer_parse_expr(syntax_analyzer* syx, smt_expr* expr, bool lhs) {
    error      err     = NULL;
    int16      tkntype = TOKEN_UNKNOWN;
    oprd_stack oprdstk;
    optr_stack optrstk;
    oprd_stack_init(&oprdstk);
    optr_stack_init(&optrstk);
    for (;;) {
        syntax_analyzer_get_token(syx);
        tkntype = syx->cur_token->token_type;

        // operands are pushed into the stack.
        if (tkntype == TOKEN_ID) {
            smt_expr* oprd = (smt_expr*)mem_alloc(sizeof(smt_expr));
            switch (syntax_analyzer_peek_token(syx)->token_type) {
            // id( => function call
            case TOKEN_OP_LPARENTHESE:
                oprd->expr_type = SMT_FUNC_CALL;
                oprd->expr.expr_func_call = (smt_func_call*)mem_alloc(sizeof(smt_func_call));
                if ((err = syntax_analyzer_parse_func_call(syx, oprd->expr.expr_func_call)) != NULL) {
                    // TODO: report error...
                }
                break;
                
            // id[ => indexing
            case TOKEN_OP_LBRACKET:
                oprd->expr_type = SMT_INDEX;
                oprd->expr.expr_index = (smt_index*)mem_alloc(sizeof(smt_index));
                if ((err = syntax_analyzer_parse_index(syx, oprd->expr.expr_index)) != NULL) {
                    // TODO: report error...
                }
                break;
                
            // just identifier
            default:
                oprd->expr_type = SMT_IDENT;
                oprd->expr.expr_ident = lex_token_getstr(syx->cur_token);
                break;
            }
            oprd_stack_push(&oprdstk, oprd);
        }
        else if (TOKEN_CONST_INTEGER <= tkntype && tkntype <= TOKEN_CONST_STRING) {
            smt_expr* oprd = (smt_expr*)mem_alloc(sizeof(smt_expr));
            oprd->expr_type = SMT_CONST_LITERAL;
            oprd->expr.expr_const_literal = (smt_const_literal*)mem_alloc(sizeof(smt_const_literal));
            oprd->expr.expr_const_literal->const_lit_type  = tkntype;
            oprd->expr.expr_const_literal->const_lit_value = lex_token_getstr(syx->cur_token);
            oprd_stack_push(&oprdstk, oprd);
        }

        // operator will be compared with the top operator of the operator stack.
        // if the operator's priority is higher than  the top operator, it will
        // be pushed into the stack. otherwise, the top operator will be used to
        // calculate the result with the top operands in the operand stack.
        else if (TOKEN_OP_SPOT <= tkntype && tkntype <= TOKEN_OP_LOGIC_OR) {
            optr cur_optr;
            cur_optr.op_token_code = tkntype;
            cur_optr.op_priority   = get_op_priority(tkntype);
            cur_optr.op_type       = syx->cur_token->extra_info;
            
            // the '(' will be directly pushed into the operator stack.
            // the unary operator like $(dereference) and @(get address) will be pushed into stack
            // as well, because the operand needed by them will be parsed later.
            if (cur_optr.op_token_code == TOKEN_OP_LPARENTHESE || cur_optr.op_type == OP_TYPE_LUNARY) {
                optr_stack_push(&optrstk, cur_optr);
                continue;
            }

            optr* top_optr;
            for (;;) {
                top_optr = optr_stack_isempty(&optrstk) == false ? optr_stack_top(&optrstk) : NULL;
                if ((top_optr == NULL) || (cur_optr.op_priority > top_optr->op_priority)) {
                    optr_stack_push(&optrstk, cur_optr);
                }
                else if (top_optr->op_type == OP_TYPE_LUNARY) {
                    if ((err = oprd_stack_calcu_once(&oprdstk, *top_optr)) != NULL) {
                        // TODO: report error...
                    }
                }
                else if (top_optr->op_token_code == TOKEN_OP_RPARENTHESE || 
                         top_optr->op_token_code == TOKEN_OP_COMMA       ||
                         top_optr->op_token_code == TOKEN_NEXT_LINE) {
                    // TODO: start parsing the expression...
                }
                else {
                    if ((err = oprd_stack_calcu_once(&oprdstk, *top_optr)) != NULL) {
                        // TODO: report error...
                    }
                }
            }
        }
    }
    oprd_stack_destroy(&oprdstk);
    optr_stack_destroy(&optrstk);
    return NULL;
}

// parse a set of expressions which separated by ','(TOKEN_OP_COMMA) operator.
error syntax_analyzer_parse_expr_list(syntax_analyzer* syx, smt_expr_list* exprlst) {
    error err   = NULL;
    bool  begin = true;
    smt_expr_list_node* cur;
    for (;;) {
        syntax_analyzer_get_token(syx);
             if ((begin == false) && (syx->cur_token->token_type == TOKEN_OP_COMMA)) {
            lex_next_token(syx->lex);
        }
        else if ((begin == false) && (syx->cur_token->token_type != TOKEN_OP_COMMA)) {
            return NULL;
        }
        else {
            // TODO: report error...
            // the fist position in the function params list can not be ','
        }

        smt_expr_list_node* create = (smt_expr_list_node*)mem_alloc(sizeof(smt_expr_list_node));
        if ((err = syntax_analyzer_parse_expr(syx, create->expr, false)) != NULL) {
            // TODO: report error...
        }

        create->next = NULL;
        if (exprlst->first != NULL) {
            cur->next = create;
            cur = create;
        }
        else {
            exprlst->first = create;
            cur = create;
        }
    }
    return NULL;
}

// parse the indexing operation.
static error syntax_analyzer_parse_index(syntax_analyzer* syx, smt_index* idx) {
    error err = NULL;
    idx->index_container = lex_token_getstr(syx->cur_token);
    lex_next_token(syx->lex); // pass the '['
    if ((err = syntax_analyzer_parse_expr(syx, &idx->index_idxexpr, true)) != NULL) {
        // TODO: report error...
    }
    lex_next_token(syx->lex); // pass the ']'
    return NULL;
}

static error syntax_analyzer_parse_decl(syntax_analyzer* syx, smt_identified_obj* decl_type, smt_ident decl_name) {
    error    err = NULL;
    smt_decl decl;
    decl.decl_type = decl_type;
    decl.decl_name = decl_name;
    syntax_analyzer_get_token(syx);
    if (syx->cur_token->token_type == TOKEN_OP_ASSIGN) {
        lex_next_token(syx->lex);
        if ((err = syntax_analyzer_parse_expr(syx, &decl.decl_init, false)) != NULL) {
            // TODO: report error...
        }
    }
    // TODO: pass the smt_decl to the semantic analyzer...
    return NULL;
}

static error syntax_analyzer_parse_assign(syntax_analyzer* syx, smt_identified_obj* assign_obj) {
    error      err = NULL;
    // TODO: pass the smt_assign to the semantic analyzer...
    return NULL;
}

static error syntax_analyzer_parse_branch_if(syntax_analyzer* syx) {
    error  err = NULL;
    smt_if _if;
    if ((err = syntax_analyzer_parse_expr(syx, &_if.if_cond, false)) != NULL) {
        // TODO: report error...
    }
    if ((err = syntax_analyzer_parse_block(syx)) != NULL) {
        // TODO: report error...
    }
    // TODO: start to compile the if branch statment...
    
    return NULL;
}

static error syntax_analyzer_parse_branch_switch(syntax_analyzer* syx) {
    return NULL;
}

static error syntax_analyzer_parse_loop(syntax_analyzer* syx) {
    return NULL;
}

static error syntax_analyzer_parse_func_def(syntax_analyzer* syx) {
    return NULL;
}

static error syntax_analyzer_parse_func_call(syntax_analyzer* syx, smt_func_call* call) {
    error err = NULL;
    call->func_name = lex_token_getstr(syx->cur_token);
    lex_next_token(syx->lex); // pass the '('
    if ((err = syntax_analyzer_parse_expr_list(syx, &call->param_passin)) != NULL) {
        // TODO: report error...
    }
    if (syntax_analyzer_peek_token(syx)->token_type != TOKEN_OP_RPARENTHESE) {
        // TODO: report error...
    }
    lex_next_token(syx->lex); // pass the ')'
    return NULL;
}

static error syntax_analyzer_parse_type(syntax_analyzer* syx) {
    return NULL;
}

static error syntax_analyzer_parse_new(syntax_analyzer* syx) {
    return NULL;
}

static error syntax_analyzer_parse_error(syntax_analyzer* syx) {
    return NULL;
}

static error syntax_analyzer_parse_deal(syntax_analyzer* syx) {
    return NULL;
}

static error syntax_analyzer_parse_block(syntax_analyzer* syx) {
    error err = NULL;
    // TODO: write a '{' to the target file
    for (;;) {
        syntax_analyzer_get_token(syx);
        if (token_iskeyword(syx->cur_token->token_type)) {
            switch (syx->cur_token->token_type) {
            case TOKEN_KEYWORD_IF:
                lex_next_token(syx->lex);
                if ((err = syntax_analyzer_parse_branch_if(syx)) != NULL) {
                    // TODO: report error...
                }
                break;

            case TOKEN_KEYWORD_SWITCH:
                lex_next_token(syx->lex);
                if ((err = syntax_analyzer_parse_branch_switch(syx)) != NULL) {
                    // TODO: report error...
                }
                break;

            case TOKEN_KEYWORD_FOR:
                lex_next_token(syx->lex);
                if ((err = syntax_analyzer_parse_loop(syx)) != NULL) {
                    // TODO: report error...
                }
                break;

            case TOKEN_KEYWORD_FUNC:
                lex_next_token(syx->lex);
                if ((err = syntax_analyzer_parse_func_def(syx)) != NULL) {
                    // TODO: report error...
                }
                break;

            case TOKEN_KEYWORD_TYPE:
                lex_next_token(syx->lex);
                if ((err = syntax_analyzer_parse_type(syx)) != NULL) {
                    // TODO: report error...
                }
                break;
            
            case TOKEN_KEYWORD_NEW:
                lex_next_token(syx->lex);
                if ((err = syntax_analyzer_parse_new(syx)) != NULL) {
                    // TODO: report error...
                }
                break;

            case TOKEN_KEYWORD_ERROR:
                lex_next_token(syx->lex);
                if ((err = syntax_analyzer_parse_error(syx)) != NULL) {
                    // TODO: report error...
                }
                break;

            case TOKEN_KEYWORD_DEAL:
                lex_next_token(syx->lex);
                if ((err = syntax_analyzer_parse_deal(syx)) != NULL) {
                    // TODO: report error...
                }
                break;

            default:
                // TODO: report error...
                break;
            }
        }
        // TODO: else if -> try to parse expression if the token is not a keyword
        // TODO: else -> report error...
    }
    // TODO: write a '}' to the target file
    return NULL;
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
                if (ERROR_CODE(err) == SYNTAX_ERROR_DEPENDENCE_NEEDED) {
                    // TODO: means some dependence files have not been compiled yet...
                }
                else {
                    // TODO: report error...
                }
            }
            if ((err = syntax_analyzer_parse_module(syx)) != NULL) {
                if (ERROR_CODE(err) == SYNTAX_ERROR_DEPENDENCE_NEEDED) {
                    // TODO: means some dependence files have not been compiled yet...
                }
                else {
                    // TODO: report error...
                }
            }
            
            // parsing the global block of the source file.
            if ((err = syntax_analyzer_parse_block(syx)) != NULL) {
                // TODO: report error...
            }
            
            // TODO: don't forget to call file_stack_pop() when the parsing is over.
            
            lex_destroy(&lex);
        }
    }
    
    return NULL;
}

void syntax_analyzer_destroy(syntax_analyzer* syx) {
    close_counter_destroy(&syx->clsctr);
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