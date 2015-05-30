/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "syntax.h"
#include "semantic.h"

static error err = NULL;

#define syntax_analyzer_get_token(syx) \
err = lex_parse_token(syx->lex);            \
if (err != NULL) {                          \
    if (ERROR_CODE(err) == LEX_ERROR_EOF) { \
        return NULL;                        \
    }                                       \
    return err;                             \
}                                           \
syx->cur_token = lex_read_token(syx->lex);

#define check_lex_error(err) \
if (ERROR_CODE(err) == LEX_ERROR_EOF) { \
    return NULL;                        \
}                                       \
return err;

// return NULL if peeking the next token failed.
static lex_token* syntax_analyzer_peek_token(syntax_analyzer* syx) {
    error err = lex_parse_token(syx->lex);
    if (err != NULL) {
        return NULL;
    }
    return lex_read_token(syx->lex);
}

// print and count the error. if the number of errors exceeds 50, the compiler
// will stop working and notify the programmer.
static void syntax_analyzer_report_error(syntax_analyzer* syx, char* errmsg) {
    syx->err_count++;
    printf("#%03d file(%s) line(%d) col(%d):\r\n     %s\r\n",
        syx->err_count,
        syx->lex->pos_file,
        syx->lex->pos_line,
        syx->lex->pos_col,
        errmsg
    );
    if (syx->err_count >= 50) {
        fprintf(stderr, "the number of errors exceeds 50. please solve the errors founded already.");
        exit(EXIT_FAILURE);
    }
}

void syntax_analyzer_init(syntax_analyzer* syx, char* file_name) {
    close_counter_init(&syx->clsctr);
    file_stack_init(&syx->file_wait_compiled);
    file_stack_push(&syx->file_wait_compiled, file_name);
    file_tree_init(&syx->file_have_compiled);
    syx->cur_token = NULL;
    syx->lex       = NULL;
    syx->smt       = NULL;
    syx->err_count = 0;
}

// parsing include statement set of the source file.
static error syntax_analyzer_parse_include(syntax_analyzer* syx) {
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
            case TOKEN_LINEFEED:
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
            lex_next_token(syx->lex);
            smt_expr* oprd = (smt_expr*)mem_alloc(sizeof(smt_expr));

            switch (syntax_analyzer_peek_token(syx)->token_type) {
            // id( => function call
            case TOKEN_OP_LPARENTHESE:
                oprd->expr_type = SMT_FUNC_CALL;
                oprd->expr.expr_func_call = (smt_func_call*)mem_alloc(sizeof(smt_func_call));
                if ((err = syntax_analyzer_parse_func_call(syx, oprd->expr.expr_func_call)) != NULL) {
                    check_lex_error(err);
                }
                oprd_stack_push(&oprdstk, oprd);
                break;

            // id id => declaration
            case TOKEN_ID:
                oprd->expr_type = SMT_IDENT;
                oprd->expr.expr_ident = lex_token_getstr(syx->cur_token);
                oprd_stack_push(&oprdstk, oprd);
                if ((err = oprd_stack_calcu(&oprdstk, &optrstk)) != NULL) {
                    syntax_analyzer_report_error(syx, err);
                }
                return NULL;

            // id[ => indexing
            case TOKEN_OP_LBRACKET:
                oprd->expr_type = SMT_INDEX;
                oprd->expr.expr_index = (smt_index*)mem_alloc(sizeof(smt_index));
                if ((err = syntax_analyzer_parse_index(syx, oprd->expr.expr_index)) != NULL) {
                    check_lex_error(err);
                }
                oprd_stack_push(&oprdstk, oprd);
                break;

            // just identifier
            default:
                oprd->expr_type = SMT_IDENT;
                oprd->expr.expr_ident = lex_token_getstr(syx->cur_token);
                oprd_stack_push(&oprdstk, oprd);
                break;
            }
        }
        else if (TOKEN_CONST_INTEGER <= tkntype && tkntype <= TOKEN_CONST_STRING) {
            lex_next_token(syx->lex);
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
                    break;
                }
                else if (top_optr->op_type == OP_TYPE_LUNARY) {
                    if ((err = oprd_stack_calcu_once(&oprdstk, *top_optr)) != NULL) {
                        syntax_analyzer_report_error(syx, err);
                    }
                }
                else if (cur_optr.op_type = OP_TYPE_EXPR_END) {
                    if ((err = oprd_stack_calcu(&oprdstk, &optrstk)) != NULL) {
                        syntax_analyzer_report_error(syx, err);
                    }
                }
                else {
                    if ((err = oprd_stack_calcu_once(&oprdstk, *top_optr)) != NULL) {
                        syntax_analyzer_report_error(syx, err);
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
        if ((err = syntax_analyzer_parse_expr(syx, &create->expr, false)) != NULL) {
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
    idx->index_container = lex_token_getstr(syx->cur_token);
    lex_next_token(syx->lex);
    close_counter_increase(&syx->clsctr, '[');
    if ((err = syntax_analyzer_parse_expr(syx, &idx->index_idxexpr, true)) != NULL) {
        // TODO: report error...
    }
    if (syntax_analyzer_peek_token(syx)->token_type != TOKEN_OP_RBRACKET) {
        // TODO: report error...
    }
    lex_next_token(syx->lex);
    if ((err = close_counter_decrease(&syx->clsctr, ']')) != NULL) {
        // TODO: report error...
    }
    return NULL;
}

static error syntax_analyzer_parse_decl(syntax_analyzer* syx, smt_expr* decl_type) {
    smt_decl decl;
    decl.decl_type = decl_type;
    decl.decl_name = lex_token_getstr(syntax_analyzer_peek_token(syx));
    lex_next_token(syx->lex);

    if (syntax_analyzer_peek_token(syx)->token_type != TOKEN_OP_ASSIGN) {
        decl.decl_init = NULL;
    }
    else {
        lex_next_token(syx->lex);
        decl.decl_init = (smt_expr*)mem_alloc(sizeof(smt_expr));
        if ((err = syntax_analyzer_parse_expr(syx, decl.decl_init, false)) != NULL) {
            check_lex_error(err);
        }
    }
    return NULL;
}

static error syntax_analyzer_parse_assign(syntax_analyzer* syx, smt_expr* expr_lhs) {
    lex_next_token(syx->lex); // pass the '='
    smt_expr expr_rhs;
    if ((err = syntax_analyzer_parse_expr(syx, &expr_rhs, false)) != NULL) {
        // TODO: report error...
    }
    smt_analyzer_parse_assign(syx->smt, expr_lhs, &expr_rhs);
    return NULL;
}

static error syntax_analyzer_parse_assigns(syntax_analyzer* syx, smt_expr_list* exprs_lhs) {
    lex_next_token(syx->lex); // pass the '='
    smt_expr_list exprs_rhs;
    if ((err = syntax_analyzer_parse_expr_list(syx, &exprs_rhs)) != NULL) {
        // TODO: report error...
    }
    smt_analyzer_parse_assigns(syx->smt, exprs_lhs, &exprs_rhs);
    return NULL;
}

// parse the if statement of the if-branch statement.
static error syntax_analyzer_parse_if(syntax_analyzer* syx, smt_if* _if) {
    if ((err = syntax_analyzer_parse_expr(syx, &_if->if_cond, false)) != NULL) {
        // TODO: report error...
    }
    if (syntax_analyzer_peek_token(syx)->token_type != TOKEN_OP_LBRACE) {
        // TODO: report error...
    }
    lex_next_token(syx->lex);
    close_counter_increase(&syx->clsctr, '{');
    if ((err = syntax_analyzer_parse_block(syx)) != NULL) {
        // TODO: report error...
    }
    return NULL;
}

// parse the ef statement of the if-branch statement.
static error syntax_analyzer_parse_ef(syntax_analyzer* syx, smt_ef* _ef) {
    if ((err = syntax_analyzer_parse_expr(syx, &_ef->ef_cond, false)) != NULL) {
        // TODO: report error...
    }
    if (syntax_analyzer_peek_token(syx)->token_type != TOKEN_OP_LBRACE) {
        // TODO: report error...
    }
    lex_next_token(syx->lex);
    close_counter_increase(&syx->clsctr, '{');
    if ((err = syntax_analyzer_parse_block(syx)) != NULL) {
        // TODO: report error...
    }
    return NULL;
}

// parse the else statement of the if-branch statement.
static error syntax_analyzer_parse_else(syntax_analyzer* syx, smt_else* _else) {
    if (syntax_analyzer_peek_token(syx)->token_type != TOKEN_OP_LBRACE) {
        // TODO: report error...
    }
    lex_next_token(syx->lex);
    close_counter_increase(&syx->clsctr, '{');
    if ((err = syntax_analyzer_parse_block(syx)) != NULL) {
        // TODO: report
    }
    return NULL;
}

static error syntax_analyzer_parse_branch_if(syntax_analyzer* syx) {
    smt_if _if;
    if ((err = syntax_analyzer_parse_if(syx, &_if)) != NULL) {
        // TODO: report error...
    }
    
    smt_ef_list       ef_list;
    smt_ef_list_node* cur;
    for (cur = ef_list.first; syntax_analyzer_peek_token(syx)->token_type == TOKEN_KEYWORD_EF;) {
        smt_ef_list_node* create = (smt_ef_list_node*)mem_alloc(sizeof(smt_ef_list_node));
        if ((err = syntax_analyzer_parse_ef(syx, &create->_ef)) != NULL) {
            // TODO: report error...
        }
        
        create->next = NULL;
        if (cur != NULL) {
            cur->next = create;
            cur = create;
        }
        else {
            ef_list.first = create;
            cur = create;
        }
    }
    
    smt_else _else;
    if ((err = syntax_analyzer_parse_else(syx, &_else)) != NULL) {
        // TODO: report error...
    }
    
    return NULL;
}

static error syntax_analyzer_parse_branch_switch(syntax_analyzer* syx) {
    smt_switch _switch;
    if ((err = syntax_analyzer_parse_expr(syx, &_switch.option, false)) != NULL) {
        // TODO: report error...
    }
    // smt_analyzer_parse_switch(smt, _switch)
    if (syntax_analyzer_peek_token(syx)->token_type != TOKEN_OP_LBRACE) {
        // TODO: report error...
    }
    lex_next_token(syx->lex);
    
    // parsing case and default statement in this loop
    bool default_exist = false;
    for (;;) {
        syntax_analyzer_get_token(syx);
        switch (syx->cur_token->token_type) {
        case TOKEN_KEYWORD_CASE: {
                lex_next_token(syx->lex);
                smt_switch_case _switch_case;
                if ((err = syntax_analyzer_parse_expr(syx, &_switch_case.option_value, false)) != NULL) {
                    // TODO: report error...
                }
                if (syntax_analyzer_peek_token(syx)->token_type != TOKEN_OP_COLON) {
                    // TODO: report error...
                }
                // smt_analyzer_parse_switch_case(smt, _switch_case);
                lex_next_token(syx->lex);
                if ((err = syntax_analyzer_parse_block(syx)) != NULL) {
                    // TODO: report error...
                }
            }
            break;
            
        case TOKEN_KEYWORD_DEFAULT: {
                if (default_exist == true) {
                    // TODO: report error...
                }
                lex_next_token(syx->lex);
                smt_switch_default _switch_default;
                if (syntax_analyzer_peek_token(syx)->token_type != TOKEN_OP_COLON) {
                    // TODO: report error...
                }
                // smt_analyzer_parse_switch_default(smt, _switch_default);
                lex_next_token(syx->lex);
                if ((err = syntax_analyzer_parse_block(syx)) != NULL) {
                    // TODO: report error...
                }
            }
            break;
            
        case TOKEN_OP_RBRACE:
            if ((err = close_counter_decrease(&syx->clsctr, '}')) != NULL) {
                // TODO: report error...
            }
            lex_next_token(syx->lex);
            break;
            
        default:
            // TODO: report error...
            break;
        }
    }
    
    return NULL;
}

static error syntax_analyzer_parse_loop(syntax_analyzer* syx) {
    if (syntax_analyzer_peek_token(syx)->token_type == TOKEN_OP_LBRACE) {
        lex_next_token(syx->lex);
        smt_loop_infinite loop_infinite;
        if ((err = syntax_analyzer_parse_block(syx)) != NULL) {
            // TODO: report error...
        }
        return NULL;
    }
    if (syntax_analyzer_peek_token(syx)->token_type == TOKEN_OP_SEMC) {
        lex_next_token(syx->lex);
    }
    
    smt_expr expr1;
    if ((err = syntax_analyzer_parse_expr(syx, &expr1, true)) != NULL) {
        // TODO: report error...
    }
    switch (syntax_analyzer_peek_token(syx)->token_type) {
    case TOKEN_OP_ASSIGN:
        break;
    case TOKEN_OP_LBRACE:
        break;
    case TOKEN_OP_COMMA:
        break;
    case TOKEN_OP_COLON:
        break;
    }
    
    smt_expr expr2;
    if ((err = syntax_analyzer_parse_expr(syx, &expr2, true)) != NULL) {
        // TODO: report error...
    }
    
    return NULL;
}

// this function is used to parse the formal parameters list of function definition.
static error syntax_analyzer_parse_formal_params(syntax_analyzer* syx, formal_param* head) {
    formal_param* tail = NULL;
    if (syntax_analyzer_peek_token(syx)->token_type != TOKEN_OP_LPARENTHESE) {
        // TODO: report error...
    }
    lex_next_token(syx->lex);
    for (;;) {
        if (syntax_analyzer_peek_token(syx)->token_type == TOKEN_OP_RPARENTHESE) {
            lex_next_token(syx->lex);
            return NULL;
        }
        
        if (tail != NULL) {
            tail->next = (formal_param*)mem_alloc(sizeof(formal_param));
            tail->next->next = NULL;
            tail = tail->next;
        }
        else {
            head = (formal_param*)mem_alloc(sizeof(formal_param));
            head->next = NULL;
            tail = head;
        }
        
        if ((err = syntax_analyzer_parse_expr(syx, &tail->param_type, false)) != NULL) {
            // TODO: report error...
        }
        
        syntax_analyzer_get_token(syx);
        if (syx->cur_token->token_type != TOKEN_ID) {
            // TODO: report error...
        }
        tail->param_ident = lex_token_getstr(syx->cur_token);
        lex_next_token(syx->lex);
    }
    return NULL;
}

static error syntax_analyzer_parse_func_def(syntax_analyzer* syx) {
    smt_func_def func_def;
    func_def.params_passin = NULL;
    func_def.params_retout = NULL;
    
    syntax_analyzer_get_token(syx);
    if (syx->cur_token->token_type != TOKEN_ID) {
        // TODO: report error...
    }
    func_def.func_name = lex_token_getstr(syx->cur_token);
    lex_next_token(syx->lex);
    
    if ((err = syntax_analyzer_parse_formal_params(syx, func_def.params_passin)) != NULL) {
        // TODO: report error...
    }
    if ((err = syntax_analyzer_parse_formal_params(syx, func_def.params_retout)) != NULL) {
        // TODO: report error...
    }
    if ((err = syntax_analyzer_parse_block(syx)) != NULL) {
        // TODO: report error...
    }
    return NULL;
}

static error syntax_analyzer_parse_func_call(syntax_analyzer* syx, smt_func_call* call) {
    call->func_name = lex_token_getstr(syx->cur_token);
    lex_next_token(syx->lex); // pass the '('
    if ((err = syntax_analyzer_parse_expr_list(syx, &call->params_passin)) != NULL) {
        // TODO: report error...
    }
    if (syntax_analyzer_peek_token(syx)->token_type != TOKEN_OP_RPARENTHESE) {
        // TODO: report error...
    }
    lex_next_token(syx->lex); // pass the ')'
    return NULL;
}

static error syntax_analyzer_parse_type_members(syntax_analyzer* syx, member_decl* head) {
    member_decl* tail;
    for (;;) {
        syntax_analyzer_get_token(syx);
        if (syx->cur_token->token_type == TOKEN_OP_RBRACE) {
            lex_next_token(syx->lex);
            return NULL;
        }
        
        if (tail != NULL) {
            tail->next = (member_decl*)mem_alloc(sizeof(member_decl));
            tail->next->next = NULL;
            tail = tail->next;
        }
        else {
            head = (member_decl*)mem_alloc(sizeof(member_decl));
            head->next = NULL;
            tail = head;
        }
        
        switch (syx->cur_token->token_type) {
        case TOKEN_KEYWORD_IN:
            tail->access = ACCESS_IN;
            lex_next_token(syx->lex);
            break;
        case TOKEN_KEYWORD_OT:
            tail->access = ACCESS_OT;
            lex_next_token(syx->lex);
            break;
        default:
            tail->access = ACCESS_IN;
            break;
        }
        
        if ((err = syntax_analyzer_parse_expr(syx, &tail->member_type, false)) != NULL) {
            // TODO: report error...
        }
        
        syntax_analyzer_get_token(syx);
        if (syntax_analyzer_peek_token(syx)->token_type != TOKEN_ID) {
            // TODO: report error...
        }
        tail->member_name = lex_token_getstr(syx->cur_token);
        lex_next_token(syx->lex);
        
        if (syntax_analyzer_peek_token(syx)->token_type != TOKEN_LINEFEED) {
            // TODO: report error...
        }
        lex_next_token(syx->lex);
    }
}

static error syntax_analyzer_parse_type(syntax_analyzer* syx) {
    syntax_analyzer_get_token(syx);
    switch (syx->cur_token->token_type) {
    case TOKEN_ID: {
            lex_next_token(syx->lex);
            smt_type_decl type_decl;
            type_decl.type_name = lex_token_getstr(syx->cur_token);
            if (syntax_analyzer_peek_token(syx)->token_type == TOKEN_OP_ASSIGN) {
                lex_next_token(syx->lex);
                type_decl.type_bind = (smt_expr*)mem_alloc(sizeof(smt_expr));
                if ((err = syntax_analyzer_parse_expr(syx, type_decl.type_bind, false)) != NULL) {
                    // TODO: report error...
                }
            } 
            else type_decl.type_bind = NULL;
            // smt_parse_type_decl();
        }
        break;

    case TOKEN_OP_LBRACE: {
            lex_next_token(syx->lex);
            smt_type_def type_def;
            type_def.members = NULL;
            if ((err = syntax_analyzer_parse_type_members(syx, type_def.members)) != NULL) {
                // TODO: report error...
            }
            // smt_parse_type_def();
        }
        break;

    default:
        // TODO: report error...
        break;
    }
    return NULL;
}

static error syntax_analyzer_parse_new(syntax_analyzer* syx) {
    smt_new _new;
    _new.new_init_params = NULL;
    _new.new_capacity    = NULL;
    
    if ((err = syntax_analyzer_parse_expr(syx, &_new.new_type_name, false)) != NULL) {
        // TODO: report error...
    }
    
    if (syntax_analyzer_peek_token(syx)->token_type == TOKEN_OP_LPARENTHESE) {
        lex_next_token(syx->lex);
        close_counter_increase(&syx->clsctr, '(');
        _new.new_init_params = (smt_expr_list*)mem_alloc(sizeof(smt_expr_list));
        _new.new_init_params->first = NULL;
        if ((err = syntax_analyzer_parse_expr_list(syx, _new.new_init_params)) != NULL) {
            // TODO: report error...
        }
    }
    
    if (syntax_analyzer_peek_token(syx)->token_type == TOKEN_OP_LBRACKET) {
        lex_next_token(syx->lex);
        close_counter_increase(&syx->clsctr, '[');
        _new.new_capacity = (smt_expr*)mem_alloc(sizeof(smt_expr));
        if ((err = syntax_analyzer_parse_expr(syx, _new.new_capacity, false)) != NULL) {
            // TODO: report error...
        }
    }
    
    smt_analyzer_parse_new(syx->smt, &_new);
    return NULL;
}

static error syntax_analyzer_parse_error(syntax_analyzer* syx) {
    return NULL;
}

static error syntax_analyzer_parse_deal(syntax_analyzer* syx) {
    return NULL;
}

static error syntax_analyzer_parse_block(syntax_analyzer* syx) {
    for (;;) {
        syntax_analyzer_get_token(syx);
        if (token_is_keyword(syx->cur_token->token_type)) {
            switch (syx->cur_token->token_type) {
            case TOKEN_KEYWORD_IF:
                lex_next_token(syx->lex);
                if ((err = syntax_analyzer_parse_branch_if(syx)) != NULL) {
                    check_lex_error(err);
                }
                break;

            case TOKEN_KEYWORD_SWITCH:
                lex_next_token(syx->lex);
                if ((err = syntax_analyzer_parse_branch_switch(syx)) != NULL) {
                    check_lex_error(err);
                }
                break;

            case TOKEN_KEYWORD_FOR:
                lex_next_token(syx->lex);
                if ((err = syntax_analyzer_parse_loop(syx)) != NULL) {
                    check_lex_error(err);
                }
                break;

            case TOKEN_KEYWORD_FUNC:
                lex_next_token(syx->lex);
                if ((err = syntax_analyzer_parse_func_def(syx)) != NULL) {
                    check_lex_error(err);
                }
                break;

            case TOKEN_KEYWORD_TYPE:
                lex_next_token(syx->lex);
                if ((err = syntax_analyzer_parse_type(syx)) != NULL) {
                    check_lex_error(err);
                }
                break;
            
            case TOKEN_KEYWORD_NEW:
                lex_next_token(syx->lex);
                if ((err = syntax_analyzer_parse_new(syx)) != NULL) {
                    check_lex_error(err);
                }
                break;

            case TOKEN_KEYWORD_ERROR:
                lex_next_token(syx->lex);
                if ((err = syntax_analyzer_parse_error(syx)) != NULL) {
                    check_lex_error(err);
                }
                break;

            case TOKEN_KEYWORD_DEAL:
                lex_next_token(syx->lex);
                if ((err = syntax_analyzer_parse_deal(syx)) != NULL) {
                    check_lex_error(err);
                }
                break;

            default:
                syntax_analyzer_report_error(syx, "unknown keyword");
                break;
            }
        }
        else if (syx->cur_token->token_type == TOKEN_LINEFEED) {
            lex_next_token(syx->lex);
            continue;
        }
        else {
            smt_expr expr;
            if ((err = syntax_analyzer_parse_expr(syx, &expr, true)) != NULL) {
                check_lex_error(err);
            }

            switch (syntax_analyzer_peek_token(syx)->token_type) {
            case TOKEN_OP_ASSIGN:
                if ((err = syntax_analyzer_parse_assign(syx, &expr)) != NULL) {
                    check_lex_error(err);
                }
                break;

            case TOKEN_ID:
                if ((err = syntax_analyzer_parse_decl(syx, &expr)) != NULL) {
                    check_lex_error(err);
                }
                break;

            case TOKEN_OP_COMMA: {
                    smt_expr_list expr_list;
                    expr_list.first = (smt_expr_list_node*)mem_alloc(sizeof(smt_expr_list_node));
                    expr_list.first->expr = expr;
                    expr_list.first->next = NULL;
                    if ((err = syntax_analyzer_parse_expr_list(syx, &expr_list)) != NULL) {
                        check_lex_error(err);
                    }
                    if (syntax_analyzer_peek_token(syx)->token_type != TOKEN_OP_ASSIGN) {
                        syntax_analyzer_report_error(
                            syx,
                            "the expression list is the left-hand-side but without the assignment context."
                        );
                    }
                    if ((err = syntax_analyzer_parse_assigns(syx, &expr_list)) != NULL) {
                        check_lex_error(err);
                    }
                }
                break;

            default:
                smt_analyzer_parse_expr(syx->smt, &expr);
                break;
            }
        }
    }
    return NULL;
}

error syntax_analyzer_work(syntax_analyzer* syx) {
    while (file_stack_isempty(&syx->file_wait_compiled) == false) {
        char* file_name = file_stack_top(&syx->file_wait_compiled);

        // if the file has been compiled then import the data from the obj file.
        if (file_tree_exist(&syx->file_have_compiled, file_name) == true) {
            // TODO: 
        }
        // if the file has not been compiled then try to compile the file.
        else {
            smt_analyzer smt;
            smt_analyzer_init(&smt);
            
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
            
            // parse the file successfully if get here. then add the file in the cache,
            // if next time some source file includes this file, the file information will
            // be extracted from the cache.
            file_stack_pop(&syx->file_wait_compiled);
            if ((err = file_tree_add(&syx->file_have_compiled, file_name)) != NULL) {
                fprintf(stderr, "add the compiled file to the cache-tree failed...");
                exit(EXIT_FAILURE);
            }

            lex_destroy(&lex);
            smt_analyzer_destroy(&smt);
        }
    }

    return NULL;
}

void syntax_analyzer_destroy(syntax_analyzer* syx) {
    close_counter_destroy(&syx->clsctr);
    file_stack_destroy(&syx->file_wait_compiled);
    file_tree_destroy(&syx->file_have_compiled);
    if (syx->lex != NULL) lex_destroy(syx->lex);
    if (syx->smt != NULL) smt_analyzer_destroy(syx->smt);
    syx->cur_token = NULL;
    syx->lex       = NULL;
    syx->smt       = NULL;
    syx->err_count = 0;
}

#undef syntax_analyzer_get_token
#undef check_lex_error
