/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "parser.h"

static error err = NULL;

// this micro definition wraps the operation about extracting
// tokens from the lexer but without the lex_next_token call.
// so you can also use this micro to peek the next one token.
#define parserGetCurToken(parser) \
err = lexerParseToken(parser->lexer);       \
if (err != NULL) {                          \
    if (ERROR_CODE(err) == LEX_ERROR_EOF)   \
        return NULL;                        \
    else                                    \
        fatal("lexer: exceptional error."); \
}                                           \
parser->cur_token = lexerReadToken(parser->lexer);

// report and count syntax errors founded in AST building stage.
// if the number of the errors is more than 50, the work of the
// syntax parsing will be stoped and notify the programmer to
// correct errors.
void parserReportErr(Parser* parser, char* errmsg) {
    parser->err_count++;
    printf("#%02d file(%s) line(%d) column(%d):\r\n    %s\r\n",
        parser->err_count,
        parser->lexer->pos_file,
        parser->lexer->pos_line,
        parser->lexer->pos_col,
        errmsg
    );
    if (parser->err_count >= 50) {
        fprintf(stderr, "the number of errors exceeds 50. please solve the errors founded already.");
        exit(EXIT_FAILURE);
    }
}

// parse constant literals.
static ASTNodeConstLit* parserParseConstLit(Parser* parser) {
    ASTNodeConstLit* node_const = (ASTNodeConstLit*)mem_alloc(sizeof(ASTNodeConstLit));
    node_const->pos_line    = parser->lexer->pos_line;
    node_const->pos_col     = parser->lexer->pos_col;
    node_const->const_type  = parser->cur_token->token_code;
    node_const->const_value = lexTokenGetStr(parser->cur_token);
    lexerNextToken(parser->lexer);
    return node_const;
}

// parse identifiers of the C+ language.
static ASTNodeID* parserParseID(Parser* parser) {
    ASTNodeID* node_id = (ASTNodeID*)mem_alloc(sizeof(ASTNodeID));
    node_id->pos_line = parser->lexer->pos_line;
    node_id->pos_col  = parser->lexer->pos_col;
    node_id->id       = lexTokenGetStr(parser->cur_token);
    lexerNextToken(parser->lexer);
    return node_id;
}

// parse expressions of the C+ language.
static ASTNodeExpr* parserParseExpr(Parser* parser) {
    OprdStack oprdstk;
    OptrStack optrstk;
    oprdStackInit(&oprdstk);
    optrStackInit(&optrstk);
    for (;;) {
        parserGetCurToken(parser);

        // operands are pushed into the stack.
        if (tokenIsID(parser->cur_token->token_code)) {
            lexerNextToken(parser->lexer);
            ASTNodeExpr* oprd = (ASTNodeExpr*)mem_alloc(sizeof(ASTNodeExpr));

            switch (parser->cur_token->token_code) {
            // id( => function call
            case TOKEN_OP_LPARENTHESE:
                oprd->expr_type = AST_NODE_FUNC_CALL;
                oprd->expr.expr_func_call = parserParseFuncCall(parser);
                oprdStackPush(&oprdstk, oprd);
                return NULL;

            // id id => declaration
            case TOKEN_ID:
                oprd->expr_type = AST_NODE_ID;
                oprd->expr.expr_id = parserParseID(parser);
                oprdStackCalcu(&oprdstk, &optrstk);
                return oprdStackGetResult(&oprdstk);

            // id[ => indexing
            case TOKEN_OP_LBRACKET:
                oprd->expr_type = AST_NODE_INDEX;
                oprd->expr.expr_index = parserParseIndex(parser);
                oprdStackPush(&oprdstk, oprd);
                break;

            // just identifier
            default:
                oprd->expr_type = AST_NODE_ID;
                oprd->expr.expr_id = parserParseID(parser);
                oprdStackPush(&oprdstk, oprd);
                break;
            }
        }
        else if (tokenIsConstLit(parser->cur_token->token_code)) {
            lexerNextToken(parser->lexer);
            ASTNodeExpr* oprd = (ASTNodeExpr*)mem_alloc(sizeof(ASTNodeExpr));
            oprd->expr_type = AST_NODE_CONST_LIT;
            oprd->expr.expr_const_lit = parserParseConstLit(parser);
            oprdStackPush(&oprdstk, oprd);
        }

        // operators will be processed based on the previous operators.
        else if (TOKEN_OP_SPOT <= parser->cur_token->token_code && parser->cur_token->token_code <= TOKEN_OP_LOGIC_OR) {
            OptrInfo cur_optr;
            cur_optr.op_token_code = parser->cur_token->token_code;
            cur_optr.op_type       = parser->cur_token->extra_info;
            cur_optr.op_priority   = getOptrPriority(parser->cur_token->token_code);

            // the '(' will be directly pushed into the operator stack.
            // the unary operator like $(dereference) and @(get address) will be pushed into stack
            // as well, because the operand needed by them will be parsed later.
            if (cur_optr.op_token_code == TOKEN_OP_LPARENTHESE || cur_optr.op_token_code == OP_TYPE_LUNARY) {
                optrStackPush(&optrstk, cur_optr);
                continue;
            }

            OptrInfo* top_optr;
            for (;;) {
                top_optr = optrStackIsEmpty(&optrstk) == false ? optrStackTop(&optrstk) : NULL;
                if (top_optr == NULL || cur_optr.op_priority > top_optr->op_priority) {
                    optrStackPush(&optrstk, cur_optr);
                    break;
                }
                if (top_optr->op_type == OP_TYPE_EXPR_END) {
                    oprdStackCalcu(&oprdstk, &optrstk);
                    break;
                }
                oprdStackCalcuOnce(&oprdstk, *top_optr);
            }
        }

        else {
            oprdStackCalcu(&oprdstk, &optrstk);
            break;
        }
    }
    oprdStackDestroy(&oprdstk);
    optrStackDestroy(&optrstk);

    return oprdStackGetResult(&oprdstk);
}

// parsing the expression list which represents a set of expressions
// separated by comma.
static ASTNodeExprList* parserParseExprList(Parser* parser) {
    ASTNodeExprList*     node_expr_list = (ASTNodeExprList*)mem_alloc(sizeof(ASTNodeExprList));
    ASTNodeExprListNode* cur            = NULL;
    ASTNodeExprListNode* create         = NULL;
    for (;;) {
        create = (ASTNodeExprListNode*)mem_alloc(sizeof(ASTNodeExprListNode));
        create->expr = parserParseExpr(parser);
        create->next = NULL;

        cur != NULL ? (cur->next = create) : (node_expr_list->exprs = create);
        cur = create;

        // stop parsing if not meet the comma
        parserGetCurToken(parser);
        if (parser->cur_token->token_code != TOKEN_OP_COMMA) {
            return node_expr_list;
        }
        lexerNextToken(parser->lexer);
    }
}

// parse the indexing statement.
static ASTNodeIndex* parserParseIndex(Parser* parser) {
    ASTNodeIndex* node_index = (ASTNodeIndex*)mem_alloc(sizeof(ASTNodeIndex));
    node_index->array = parserParseID(parser);
    node_index->index = parserParseExpr(parser);
    return node_index;
}

// parsing the if statement.
static ASTNodeIf* parserParseIf(Parser* parser) {
    ASTNodeIf* node_if = (ASTNodeIf*)mem_alloc(sizeof(ASTNodeIf));
    node_if->cond        = parserParseExpr(parser);
    node_if->block       = parserParseBlock(parser);
    node_if->branch_ef   = NULL;
    node_if->branch_else = NULL;

    parserGetCurToken(parser);
    if (parser->cur_token->token_code == TOKEN_KEYWORD_EF) {
        node_if->branch_ef = parserParseEf(parser);
    }

    parserGetCurToken(parser);
    if (parser->cur_token->token_code == TOKEN_KEYWORD_ELSE) {
        node_if->branch_else = parserParseElse(parser);
    }

    return node_if;
}

// parsing the ef(else if) statement.
static ASTNodeEf* parserParseEf(Parser* parser) {
    ASTNodeEf* node_ef = NULL;
    ASTNodeEf* cur     = NULL;
    ASTNodeEf* create  = NULL;
    for (;;) {
        create = (ASTNodeEf*)mem_alloc(sizeof(ASTNodeEf));
        create->cond  = parserParseExpr(parser);
        create->block = parserParseBlock(parser);
        create->next  = NULL;

        cur != NULL ? (cur->next = create) : (node_ef = create);
        cur = create;

        parserGetCurToken(parser);
        if (parser->cur_token->token_code != TOKEN_KEYWORD_EF) {
            return node_ef;
        }
        lexerNextToken(parser->lexer);
    }
}

// parsing the else statement.
static ASTNodeElse* parserParseElse(Parser* parser) {
    ASTNodeElse* node_else = (ASTNodeElse*)mem_alloc(sizeof(ASTNodeElse));
    node_else->block = parserParseBlock(parser);
    return node_else;
}

// parsing the switch statement.
static ASTNodeSwitch* parserParseSwitch(Parser* parser) {
    ASTNodeSwitch* node_switch = (ASTNodeSwitch*)mem_alloc(sizeof(ASTNodeSwitch));
    node_switch->option         = parserParseExpr(parser);
    node_switch->branch_case    = NULL;
    node_switch->branch_default = NULL;

    parserGetCurToken(parser);
    if (parser->cur_token->token_code != TOKEN_OP_LBRACE) {
        parserReportErr(parser, "miss the { to open the switch statement's scope.");
    }
    lexerNextToken(parser->lexer);

    ASTNodeSwitchCase* cur    = NULL;
    ASTNodeSwitchCase* create = NULL;
    for (;;) {
        parserGetCurToken(parser);
        switch (parser->cur_token->token_code) {
        case TOKEN_KEYWORD_CASE:
            lexerNextToken(parser->lexer);
            if ((create = parserParseSwitchCase(parser)) == NULL) {
                return NULL;
            }
            cur != NULL ? (cur->next = create) : (node_switch->branch_case = create);
            cur = create;
            break;

        case TOKEN_KEYWORD_DEFAULT:
            lexerNextToken(parser->lexer);
            if (node_switch->branch_default == NULL) {
                node_switch->branch_default = parserParseSwitchDeft(parser);
                break;
            }
            else {
                parserReportErr(parser, "multiple default branches in one switch statment.");
                return NULL;
            }

        case TOKEN_OP_RBRACE:
            lexerReadToken(parser->lexer);
            return node_switch;

        default:
            parserReportErr(parser, "invalid token within the switch statement context.");
            return NULL;
        }
    }
}

// parsing the case branch of the switch statement.
static ASTNodeSwitchCase* parserParseSwitchCase(Parser* parser) {
    ASTNodeSwitchCase* node_switch_case = (ASTNodeSwitchCase*)mem_alloc(sizeof(ASTNodeSwitchCase));
    node_switch_case->value = parserParseExpr(parser);

    parserGetCurToken(parser);
    if (parser->cur_token->token_code != TOKEN_OP_COLON) { // pass the :
        parserReportErr(parser, "miss the : to begin the case branch of the switch statement.");
        return NULL;
    }
    lexerNextToken(parser->lexer);

    node_switch_case->body = parserParseCaseBody(parser);
    node_switch_case->next = NULL;
    return node_switch_case;
}

// parsing the default branch of the switch statement.
static ASTNodeSwitchDeft* parserParseSwitchDeft(Parser* parser) {
    ASTNodeSwitchDeft* node_switch_deft = (ASTNodeSwitchDeft*)mem_alloc(sizeof(ASTNodeSwitchDeft));
    node_switch_deft->block = parserParseBlock(parser);
    return node_switch_deft;
}

static ASTNode* parserParseLoop(Parser* parser) {
    return NULL;
}

static ASTNodeLoopFor* parserParseLoopFor(Parser* parser) {
    return NULL;
}

// parsing function call statement.
static ASTNodeFuncCall* parserParseFuncCall(Parser* parser) {
    ASTNodeFuncCall* node_func_call = (ASTNodeFuncCall*)mem_alloc(sizeof(ASTNodeFuncCall));
    node_func_call->func_name   = parserParseID(parser);
    node_func_call->func_params = parserParseExprList(parser);
    return node_func_call;
}

// parsing block which represents a set of statements between
// a couple of braces.
static ASTNodeBlock* parserParseBlock(Parser* parser) {

    // check the '{' which is the start symbol of a Block
    parserGetCurToken(parser);
    if (parser->cur_token->token_code != TOKEN_OP_LBRACE) {
        parserReportErr(parser, "miss a left-brace '{' to open new scope.");
        return NULL;
    }
    lexerNextToken(parser->lexer);

    ASTNodeBlock* node_block = (ASTNodeBlock*)mem_alloc(sizeof(ASTNodeBlock));
    ASTNodeStmt*  stmt_cur   = NULL;
    ASTNodeStmt*  stmt_new   = NULL;
    for (;;) {
        parserGetCurToken(parser);
        if (tokenIsKeywords(parser->cur_token->token_code)) {
            switch (parser->cur_token->token_code) {
            case TOKEN_KEYWORD_IF:
                // stmt_new  = parserParseIf(parser);
                stmt_cur != NULL ? (stmt_cur->next = stmt_new) : (node_block->stmts = stmt_new);
                stmt_cur  = stmt_new;
                break;

            case TOKEN_KEYWORD_SWITCH:
                // stmt_new  = parserParseSwitch(parser);
                stmt_cur != NULL ? (stmt_cur->next = stmt_new) : (node_block->stmts = stmt_new);
                stmt_cur  = stmt_new;
                break;

            case TOKEN_KEYWORD_FOR:
                // stmt_new  = parserParseLoop(parser);
                stmt_cur != NULL ? (stmt_cur->next = stmt_new) : (node_block->stmts = stmt_new);
                stmt_cur  = stmt_new;
                break;

            case TOKEN_KEYWORD_BREAK:
                // stmt_new  = parserParseSwitch(parser);
                stmt_cur != NULL ? (stmt_cur->next = stmt_new) : (node_block->stmts = stmt_new);
                stmt_cur  = stmt_new;
                break;

            case TOKEN_KEYWORD_CONTINUE:
                // stmt_new  = parserParseSwitch(parser);
                stmt_cur != NULL ? (stmt_cur->next = stmt_new) : (node_block->stmts = stmt_new);
                stmt_cur  = stmt_new;
                break;

            case TOKEN_KEYWORD_TYPE:
                // stmt_new  = parserParseSwitch(parser);
                stmt_cur != NULL ? (stmt_cur->next = stmt_new) : (node_block->stmts = stmt_new);
                stmt_cur  = stmt_new;
                break;

            case TOKEN_KEYWORD_FUNC:
                break;

            case TOKEN_KEYWORD_NEW:
                break;

            case TOKEN_KEYWORD_RETURN:
                break;

            case TOKEN_KEYWORD_ERROR:
                break;

            case TOKEN_KEYWORD_DEAL:
                break;

            default:
                parserReportErr(parser, "unacceptable keyword in block context.");
                break;
            }
        }
        else if (parser->cur_token->token_code == TOKEN_OP_LBRACE) {
            stmt_cur != NULL ? (stmt_cur->next = stmt_new) : (node_block->stmts = stmt_new);
            stmt_cur  = stmt_new;
            continue;
        }
        else if (parser->cur_token->token_code == TOKEN_LINEFEED) {
            lexerNextToken(parser->lexer);
            continue;
        }
        else if (parser->cur_token->token_code == TOKEN_OP_RBRACE) {
            lexerNextToken(parser->lexer);
            return node_block;
        }
        else {
            /*
             expr id   -> declaration
             expr =    -> assignment
             expr ,    -> expression list
             */
            ASTNodeExpr* node_expr = parserParseExpr(parser);
        }
    }
}

static ASTNodeCaseBody* parserParseCaseBody(Parser* parser) {
    ASTNodeCaseBody* node_case_body = (ASTNodeCaseBody*)mem_alloc(sizeof(ASTNodeCaseBody));
    ASTNodeStmt*     stmt_cur       = NULL;
    ASTNodeStmt*     stmt_new       = NULL;

    for (;;) {
        parserGetCurToken(parser);
        if (tokenIsKeywords(parser->cur_token->token_code)) {
            switch (parser->cur_token->token_code) {
            case TOKEN_KEYWORD_IF:
                break;

            case TOKEN_KEYWORD_SWITCH:
                break;

            case TOKEN_KEYWORD_FOR:
                break;

            case TOKEN_KEYWORD_BREAK:
                break;

            case TOKEN_KEYWORD_CONTINUE:
                break;
                
            case TOKEN_KEYWORD_FTHROUGH:
                break;

            case TOKEN_KEYWORD_TYPE:
                break;

            case TOKEN_KEYWORD_FUNC:
                break;

            case TOKEN_KEYWORD_NEW:
                break;

            case TOKEN_KEYWORD_RETURN:
                break;

            case TOKEN_KEYWORD_ERROR:
                break;

            case TOKEN_KEYWORD_DEAL:
                break;

            case TOKEN_KEYWORD_CASE:
            case TOKEN_KEYWORD_DEFAULT:
                return node_case_body;

            default:
                parserReportErr(parser, "unacceptable keyword in case context.");
                break;
            }
        }
        else if (parser->cur_token->token_code == TOKEN_OP_RBRACE) {
            return node_case_body;
        }
        else if (parser->cur_token->token_code == TOKEN_OP_LBRACE) {
            
        }
        else if (parser->cur_token->token_code == TOKEN_LINEFEED) {
            lexerNextToken(parser->lexer);
        }
        else {
            // TODO: parsing expressions
        }
    }
    return NULL;
}

static ASTNodeGlobalScope* parserParseGlobalScope(Parser* parser) {
    ASTNodeGlobalScope* node_global_scope = (ASTNodeGlobalScope*)mem_alloc(sizeof(ASTNodeGlobalScope));
    ASTNodeStmt*        stmt_cur          = NULL;
    ASTNodeStmt*        stmt_new          = NULL;

    for (;;) {
        parserGetCurToken(parser);
        if (tokenIsKeywords(parser->cur_token->token_code)) {
            switch (parser->cur_token->token_code) {
            case TOKEN_KEYWORD_TYPE:
                break;
                
            case TOKEN_KEYWORD_FUNC:
                break;
                
            case TOKEN_KEYWORD_MODULE:
                break;
                
            case TOKEN_KEYWORD_INCLUDE:
                break;
                
            default:
                parserReportErr(parser, "unacceptable keyword in the global scope context.");
                break;
            }
        }
        else {
            // TODO: parsing expressions
        }
    }
    return NULL;
}

void parserInit(Parser* parser) {
    fileStackInit(&parser->file_wait_compiled);
    fileTreeInit (&parser->file_have_compiled);
    parser->lexer     = NULL;
    parser->ast       = NULL;
    parser->cur_token = NULL;
    parser->cur_scope = NULL;
    parser->err_count = 0;
}

error parserStart(Parser* parser, char* main_file) {
    fileStackPush(&parser->file_wait_compiled, main_file);
    for (;;) {
        if (fileStackIsEmpty(&parser->file_wait_compiled) == true) {
            return NULL;
        }
        char* file = fileStackTop(&parser->file_wait_compiled);
        
        if (fileTreeExist(&parser->file_have_compiled, file) == true) {
            // TODO: the file has been compiled...
            //       just use it directory.
        }
        else {
            
        }
        
        fileStackPop(&parser->file_wait_compiled);
    }
}

void parserDestroy(Parser* parser) {
    fileStackDestroy(&parser->file_wait_compiled);
    fileTreeDestroy (&parser->file_have_compiled);

    if (parser->ast != NULL)
        astDestroy(parser->ast);
    if (parser->lexer != NULL) 
        lexerDestroy(parser->lexer);
    if (parser->cur_scope != NULL)
        scopeDestroy(parser->cur_scope);

    parser->lexer     = NULL;
    parser->ast       = NULL;
    parser->cur_scope = NULL;
    parser->cur_token = NULL;
    parser->err_count = 0;
}

/*
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
}*/

#undef parserGetCurToken
#undef parserCheckRetNode
