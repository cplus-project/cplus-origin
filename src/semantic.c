/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "semantic.h"

void smt_analyzer_init(smt_analyzer* smt) {
    smt->cur_scp = NULL;
}

void smt_analyzer_scope_open(smt_analyzer* smt) {
    scope* scp = (scope*)mem_alloc(sizeof(scope));
    scope_init(scp, smt->cur_scp);
}

void smt_analyzer_scope_close(smt_analyzer* smt) {
    scope* temp = smt->cur_scp;
    smt->cur_scp = smt->cur_scp->outer;
    scope_destroy(temp);
}

error smt_analyzer_parse_ident(smt_analyzer* smt) {
    
}

error smt_analyzer_parse_expr(smt_analyzer* smt, smt_expr* expr) {
    if (expr->expr_type < 0x09) {
        switch (expr->expr_type) {
        case SMT_CONST_LITERAL:
            break;
        case SMT_IDENT:
            break;
        case SMT_EXPR_BINARY:
            break;
        case SMT_EXPR_UNARY:
            break;
        }
    }
    else {
        switch (expr->expr_type) {
        case SMT_FUNC_CALL:
            break;
        case SMT_INDEX:
            break;
        case SMT_TYPE_DEF:
            break;
        case SMT_FUNC_DEF:
            break;
        case SMT_NEW:
            break;
        }
    }
}

error smt_analyzer_parse_assign(smt_analyzer* smt, smt_expr* expr_lhs, smt_expr* expr_rhs) {
    return NULL;
}

error smt_analyzer_parse_assigns(smt_analyzer* smt, smt_expr_list* exprs_lhs, smt_expr_list* exprs_rhs) {
    return NULL;
}

error smt_analyzer_parse_new(smt_analyzer* smt, smt_new* _new) {
    return NULL;
}

void smt_analyzer_destroy(smt_analyzer* smt) {

}
