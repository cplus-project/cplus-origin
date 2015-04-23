/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "ast.h"

/****** methods of include_list ******/

void include_list_init(include_list* icldlist) {
    icldlist->head = NULL;
    icldlist->tail = NULL;
}

void include_list_add(include_list* icldlist, char* file) {
    include_list_node* create = (include_list_node*)mem_alloc(sizeof(include_list_node));
    create->file = file;
    if (icldlist->head != NULL) {
        icldlist->tail->next = create;
        icldlist->tail       = create;
    }
    else {
        icldlist->head = create;
        icldlist->tail = create;
    }
}

// return value:
//     true  -> exist already
//     false -> not exist
bool include_list_exist(include_list* icldlist, char* file) {
    int i;
    include_list_node* ptr;
    for (ptr = icldlist->head; ptr != NULL; ptr = ptr->next) {
        for (i = 0; ; i++) {
            if (ptr->file[i] == '\0' && file[i] == '\0') {
                return true;
            }
            if (ptr->file[i] != file[i]) {
                break;
            }
        }
    }
    return false;
}

void include_list_destroy(include_list* icldlist) {
    include_list_node* temp;
    icldlist->tail = icldlist->head;
    for (;;) {
        if (icldlist->tail == NULL) {
            icldlist->head =  NULL;
            return;
        }
        temp = icldlist->tail;
        icldlist->tail = icldlist->tail->next;
        mem_free(temp);
    }
}

/****** methods of module_list ******/

void module_list_init(module_list* modlist) {
    modlist->head = NULL;
    modlist->tail = NULL;
}

void module_list_add(module_list* modlist, char* module) {
    module_list_node* create = (module_list_node*)mem_alloc(sizeof(module_list_node));
    create->module = module;
    create->next   = NULL;
    if (modlist->head != NULL) {
        modlist->tail->next = create;
        modlist->tail       = create;
    }
    else {
        modlist->head = create;
        modlist->tail = create;
    }
}

// return value:
//     true  -> exist already
//     false -> not exist
bool module_list_exist(module_list* modlist, char* module) {
    int i;
    module_list_node* ptr;
    for (ptr = modlist->head; ptr != NULL; ptr = ptr->next) {
        for (i = 0; ; i++) {
            if (ptr->module[i] == '\0' && module[i] == '\0') {
                return true;
            }
            if (ptr->module[i] != module[i]) {
                break;
            }
        }
    }
    return false;
}

void module_list_destroy(module_list* modlist) {
    module_list_node* temp = NULL;
    modlist->tail = modlist->head;
    for (;;) {
        if (modlist->tail == NULL) {
            modlist->head =  NULL;
            return;
        }
        temp = modlist->tail;
        modlist->tail = modlist->tail->next;
        mem_free(temp);
    }
}

/****** methods os ast_elem_block ******/

void ast_elem_block_init(ast_elem_block* block) {
    block->head = NULL;
    block->tail = NULL;
}

void ast_elem_block_add(ast_elem_block* block, ast_elem* elem) {
    ast_elem_block_node* create = (ast_elem_block_node*)mem_alloc(sizeof(ast_elem_block_node));
    create->elem = elem;
    create->next = NULL;
    if (block->head != NULL) {
        create->prev      = block->tail;
        block->tail->next = create;
        block->tail       = create;
    } else {
        create->prev = NULL;
        block->head  = create;
        block->tail  = create;
    }
}

// return true if the block is empty.
bool ast_elem_block_isempty(ast_elem_block* block) {
    if (block->head == NULL) {
        return true;
    }
    return false;
}

// return NULL if these are no ast element in the block.
ast_elem* ast_elem_block_get_back(ast_elem_block* block) {
    if (block->tail != NULL) {
        return block->tail->elem;
    }
    return NULL;
}

// get the back element's ast_elem_type of the block.
int8 ast_elem_block_get_backtype(ast_elem_block* block) {
    if (block->tail != NULL) {
        return block->tail->elem->ast_elem_type;
    }
    return AST_ELEM_UNKNOWN;
}

// return an error if the block is empty.
error ast_elem_block_remove_back(ast_elem_block* block) {
    if (block->tail != NULL) {
        ast_elem_block_node* temp = block->tail;
        block->tail = block->tail->prev;
        if (block->tail == NULL) {
            block->head =  NULL;
        }
        else {
            block->tail->next = NULL;
        }
        mem_free(temp);
        return NULL;
    }
    return new_error("the block is empty.");
}

// return an error if the block is empty.
error ast_elem_block_replace_back(ast_elem_block* block, ast_elem* elem) {
    if (block->tail != NULL) {
        mem_free(block->tail->elem);
        block->tail->elem = elem;
        return NULL;
    }
    return new_error("the block is empty.");
}

void ast_elem_block_destroy(ast_elem_block* block) {
    ast_elem_block_node* temp = NULL;
    for (;;) {
        if (block->head == NULL) {
            block->tail = NULL;
            return;
        }
        temp = block->head;
        block->head = block->head->next;
        mem_free(temp);
    }
}

/****** methods of ast_elem_op ******/

void ast_elem_op_get_priority(ast_elem_op* elem_op) {
         if (elem_op->op_token_code < 403) elem_op->op_priority = OP_PRIORITY_9;
    else if (elem_op->op_token_code < 406) elem_op->op_priority = OP_PRIORITY_8;
    else
        elem_op->op_priority = OP_PRIORITY_NULL;
}

/****** methods of ef_list ******/

void ef_list_init(ef_list* eflist) {
    eflist->head = NULL;
    eflist->tail = NULL;
}

void ef_list_add(ef_list* eflist, ast_elem_ef* ef) {
    ef_list_node* create = (ef_list_node*)mem_alloc(sizeof(ef_list_node));
    create->ef   = ef;
    create->next = NULL;
    if (eflist->head != NULL) {
        eflist->tail->next = create;
        eflist->tail       = create;
    }
    else {
        eflist->head = create;
        eflist->tail = create;
    }
}

void ef_list_destroy(ef_list* eflist) {
    ef_list_node* temp = NULL;
    for (;;) {
        if (eflist->head == NULL) {
            eflist->tail =  NULL;
            return;
        }
        temp = eflist->head;
        eflist->head = eflist->head->next;
        mem_free(temp);
    }
}

/****** methods of switch_case_list ******/

void switch_case_list_init(switch_case_list* caselist) {
    caselist->head = NULL;
    caselist->tail = NULL;
}

void switch_case_list_add(switch_case_list* caselist, ast_elem_switch_case* switch_case) {
    switch_case_list_node* create = (switch_case_list_node*)mem_alloc(sizeof(switch_case_list_node));
    create->switch_case = switch_case;
    create->next        = NULL;
    if (caselist->head != NULL) {
        caselist->tail->next = create;
        caselist->tail       = create;
    }
    else {
        caselist->head = create;
        caselist->tail = create;
    }
}

void switch_case_list_destroy(switch_case_list* caselist) {
    switch_case_list_node* temp = NULL;
    for (;;) {
        if (caselist->head == NULL) {
            caselist->tail =  NULL;
            return;
        }
        temp = caselist->head;
        caselist->head = caselist->head->next;
        mem_free(temp);
    }
}

/****** methods of decl_list ******/

void decl_list_init(decl_list* decllist) {
    decllist->head = NULL;
    decllist->tail = NULL;
}

void decl_list_add(decl_list* decllist, declare* decl) {
    decl_list_node* create = (decl_list_node*)mem_alloc(sizeof(decl_list_node));
    create->decl = decl;
    create->next = NULL;
    if (decllist->head != NULL) {
        decllist->tail->next = create;
        decllist->tail       = create;
    }
    else {
        decllist->head = create;
        decllist->tail = create;
    }
}

void decl_list_destroy(decl_list* decllist) {
    decl_list_node* temp = NULL;
    for (;;) {
        if (decllist->head == NULL) {
            decllist->tail =  NULL;
            return;
        }
        temp = decllist->head;
        decllist->head = decllist->head->next;
        mem_free(temp);
    }
}

/****** methods of ast_elem_stack ******/

void actual_param_list_init(actual_param_list* actparamlst) {
    actparamlst->head = NULL;
    actparamlst->tail = NULL;
}

void actual_param_list_add(actual_param_list* actparamlst, actual_param* param) {
    actual_param_list_node* create = (actual_param_list_node*)mem_alloc(sizeof(actual_param_list_node));
    create->param = param;
    create->next  = NULL;
    if (actparamlst->head != NULL) {
        actparamlst->tail->next = create;
        actparamlst->tail       = create;
    }
    else {
        actparamlst->head = create;
        actparamlst->tail = create;
    }
}

void actual_param_list_destroy(actual_param_list* actparamlst) {
    actual_param_list_node* temp = NULL;
    for (;;) {
        if (actparamlst->head == NULL) {
            actparamlst->tail =  NULL;
            return;
        }
        temp = actparamlst->head;
        actparamlst->head = actparamlst->head->next;
        mem_free(temp);
    }
}

/****** methods of deal_case_list ******/

void deal_case_list_init(deal_case_list* dealcslist) {
    dealcslist->head = NULL;
    dealcslist->tail = NULL;
}

void deal_case_list_add(deal_case_list* dealcslist, ast_elem_deal_case* deal_case) {
    deal_case_list_node* create = (deal_case_list_node*)mem_alloc(sizeof(deal_case_list_node));
    create->deal_case = deal_case;
    create->next      = NULL;
    if (dealcslist->head != NULL) {
        dealcslist->tail->next = create;
        dealcslist->tail       = create;
    }
    else {
        dealcslist->head = create;
        dealcslist->tail = create;
    }
}

void deal_case_list_destroy(deal_case_list* dealcslist) {
    deal_case_list_node* temp = NULL;
    for (;;) {
        if (dealcslist->head == NULL) {
            dealcslist->tail =  NULL;
            return;
        }
        temp = dealcslist->head;
        dealcslist->head = dealcslist->head->next;
        mem_free(temp);
    }
}

/****** methods of ast_elem_stack ******/

void ast_elem_stack_init(ast_elem_stack* stk) {
    stk->top = NULL;
}

void ast_elem_stack_push(ast_elem_stack* stk, ast_elem* elem) {
    ast_elem_stack_node* create = (ast_elem_stack_node*)mem_alloc(sizeof(ast_elem_stack_node));
    create->elem = elem;
    create->next = NULL;
    if (stk->top != NULL) {
        create->next = stk->top;
        stk->top = create;
    }
    else {
        stk->top = create;
    }
}

// note:
//    the pop() method will release the memory of the stack node without
//    the payload of the ast_elem's member ast_elem_entity.
//    so you should release the ast_elem_entity's space by yourself.
error ast_elem_stack_pop(ast_elem_stack* stk) {
    if (stk->top == NULL) {
        return new_error("err: the ast node stack is empty.");
    }
    ast_elem_stack_node* temp = stk->top;
    stk->top = stk->top->next;
    mem_free(temp->elem);
    mem_free(temp);
}

// return NULL if the stack is empty.
ast_elem* ast_elem_stack_top(ast_elem_stack* stk) {
    if (stk->top != NULL) {
        return stk->top->elem;
    }
    return NULL;
}

void ast_elem_stack_destroy(ast_elem_stack* stk) {
    ast_elem_stack_node* temp = NULL;
    for (;;) {
        if (stk->top == NULL) {
            return;
        }
        temp = stk->top;
        stk->top = stk->top->next;
        mem_free(temp);
    }
}

/****** methods of ast ******/

void ast_init(ast* astree) {
    astree->imports      = NULL;
    astree->global_block = NULL;
}

void ast_destroy(ast* astree) {
    // TODO: need implements more concretelly...
    astree->imports      = NULL;
    astree->global_block = NULL;
}
