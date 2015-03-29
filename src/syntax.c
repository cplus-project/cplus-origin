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

#define syntax_analyzer_get_token(syx) \
err = lex_parse_token(&syx->lex);           \
if (err != NULL) {                          \
    return err;                             \
}                                           \
syx->cur_token = lex_read_token(&syx->lex); \
lex_next_token(&syx->lex);

error syntax_analyzer_init(syntax_analyzer* syx, char* file, ast* astree) {
    if (file == NULL) {
        return new_error("err: no file to parse.");
    }
    if (astree == NULL) {
        return new_error("err: no ast to generate.");
    }
    lex_init(&syx->lex);
    error err = lex_open_srcfile(&syx->lex, file);
    if (err != NULL) {
        return err;
    }
    syx->cur_token = NULL;
    token_buffer_init(&syx->tkn_buff);
    syx->astree = astree;
}

static error syntax_analyzer_parse_stmt_include(syntax_analyzer* syx) {
    return NULL;
}

static error syntax_analyzer_parse_stmt_module(syntax_analyzer* syx) {
    return NULL;
}

static error syntax_analyzer_parse_stmt_decl(syntax_analyzer* syx) {
    return NULL;
}

static error syntax_analyzer_parse_stmt_assign(syntax_analyzer* syx) {
    return NULL;
}

static error syntax_analyzer_parse_stmt_if(syntax_analyzer* syx) {
    return NULL;
}

static error syntax_analyzer_parse_stmt_ef(syntax_analyzer* syx) {
    return NULL;
}

static error syntax_analyzer_parse_stmt_else(syntax_analyzer* syx) {
    return NULL;
}

static error syntax_analyzer_parse_stmt_loop_for(syntax_analyzer* syx) {
    return NULL;
}

static error syntax_analyzer_parse_stmt_loop_while(syntax_analyzer* syx) {
    return NULL;
}

static error syntax_analyzer_parse_stmt_loop_infinite(syntax_analyzer* syx) {
    return NULL;
}

static error syntax_analyzer_parse_stmt_loop_foreach(syntax_analyzer* syx) {
    return NULL;
}

static error syntax_analyzer_parse_stmt_loop_foreach_withidx(syntax_analyzer* syx) {
    return NULL;
}

static error syntax_analyzer_parse_stmt_type_decl(syntax_analyzer* syx) {
    return NULL;
}

static error syntax_analyzer_parse_stmt_type_define(syntax_analyzer* syx) {
    return NULL;
}

static error syntax_analyzer_parse_stmt_func(syntax_analyzer* syx) {
    return NULL;
}

static error syntax_analyzer_parse_stmt_return(syntax_analyzer* syx) {
    return NULL;
}

static error syntax_analyzer_parse_stmt_error(syntax_analyzer* syx) {
    return NULL;
}

static error syntax_analyzer_parse_stmt_deal_single(syntax_analyzer* syx) {
    return NULL;
}

static error syntax_analyzer_parse_stmt_deal_multi(syntax_analyzer* syx) {
    return NULL;
}

error syntax_analyzer_generate_ast(syntax_analyzer* syx) {
    error err = NULL;
    for (;;) {
        syntax_analyzer_get_token(syx);
        switch (syx->cur_token->token_type) {
        case TOKEN_KEYWORD_INCLUDE:
            err = syntax_analyzer_parse_stmt_include(syx);
            if (err != NULL) return err;
            break;

        case TOKEN_KEYWORD_MODULE:
            err = syntax_analyzer_parse_stmt_module(syx);
            if (err != NULL) return err;
            break;

        case TOKEN_ID:
            token_buffer_push(&syx->tkn_buff, *syx->cur_token);
            syntax_analyzer_get_token(syx);
            switch (syx->cur_token->token_type) {
            case TOKEN_ID:
                err = syntax_analyzer_parse_stmt_decl(syx);
                if (err != NULL) return err;
                break;
            case TOKEN_OP_ASSIGN:
                err = syntax_analyzer_parse_stmt_assign(syx);
                if (err != NULL) return err;
                break;
            case TOKEN_OP_SPOT:
                // TODO: id.method or id.property
                break;
            case TOKEN_OP_LPARENTHESE:
                // TODO: function(...
                break;
            case TOKEN_OP_LBRACKET:
                // TODO: array[...
                break;
            default:
                // TODO: report error...
                break;
            }
            break;

        case TOKEN_KEYWORD_IF:
            err = syntax_analyzer_parse_stmt_if(syx);
            if (err != NULL) return err;
            break;

        case TOKEN_KEYWORD_EF:
            err = syntax_analyzer_parse_stmt_ef(syx);
            if (err != NULL) return err;
            break;

        case TOKEN_KEYWORD_ELSE:
            err = syntax_analyzer_parse_stmt_else(syx);
            if (err != NULL) return err;
            break;

        case TOKEN_KEYWORD_FOR:
            syntax_analyzer_get_token(syx);
            switch (syx->cur_token->token_type) {
            case TOKEN_ID:
                token_buffer_push(&syx->tkn_buff, *syx->cur_token);
                syntax_analyzer_get_token(syx);
                switch (syx->cur_token->token_type) {
                // for init; cond; step {...}
                case TOKEN_OP_SEMC:
                    err = syntax_analyzer_parse_stmt_loop_for(syx);
                    if (err != NULL) return err;
                    break;
                // for cond {...}
                case TOKEN_OP_LBRACE:
                    err = syntax_analyzer_parse_stmt_loop_while(syx);
                    if (err != NULL) return err;
                    break;
                // for val : array {...}
                case TOKEN_OP_COLON:
                    err = syntax_analyzer_parse_stmt_loop_foreach(syx);
                    if (err != NULL) return err;
                    break;
                // for val, idx : array {...}
                case TOKEN_OP_COMMA:
                    err = syntax_analyzer_parse_stmt_loop_foreach_withidx(syx);
                    if (err != NULL) return err;
                    break;
                default:
                    // TODO: report error...
                    break;
                }
                break;
            // for {...}
            case TOKEN_OP_LBRACE:
                err = syntax_analyzer_parse_stmt_loop_infinite(syx);
                if (err != NULL) return err;
                break;
            default:
                // TODO: report error...
                break;
            }
            break;

        case TOKEN_KEYWORD_TYPE:
            syntax_analyzer_get_token(syx);
            switch (syx->cur_token->token_type) {
            // type typename
            case TOKEN_ID:
                err = syntax_analyzer_parse_stmt_type_decl(syx);
                if (err != NULL) return err;
                break;
            // type { members... }
            case TOKEN_OP_LBRACE:
                err = syntax_analyzer_parse_stmt_type_define(syx);
                if (err != NULL) return err;
                break;
            default:
                // TODO: report error...
                break;
            }
            break;

        case TOKEN_KEYWORD_FUNC:
            err = syntax_analyzer_parse_stmt_func(syx);
            if (err != NULL) return err;
            break;

        case TOKEN_KEYWORD_RETURN:
            err = syntax_analyzer_parse_stmt_return(syx);
            if (err != NULL) return err;
            break;

        case TOKEN_KEYWORD_ERROR:
            err = syntax_analyzer_parse_stmt_error(syx);
            if (err != NULL) return err;
            break;

        case TOKEN_KEYWORD_DEAL:
            syntax_analyzer_get_token(syx);
            switch (syx->cur_token->token_type) {
            // deal errtag {...}
            case TOKEN_ID:
                err = syntax_analyzer_parse_stmt_deal_single(syx);
                if (err != NULL) return err;
                break;
            // deal {
            // case errtag1:
            //     ...
            // case errtag2:
            //     ...
            // default:
            //     ...
            // }
            case TOKEN_OP_LBRACE:
                err = syntax_analyzer_parse_stmt_deal_multi(syx);
                if (err != NULL) return err;
                break;
            default:
                // TODO: report error...
                break;
            }
            break;
        }
    }
    return NULL;
}

void syntax_analyzer_destroy(syntax_analyzer* syx) {
    lex_destroy(&syx->lex);
    syx->cur_token = NULL;
    token_buffer_destroy(&syx->tkn_buff);
    syx->astree = NULL;
}
