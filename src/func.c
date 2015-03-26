/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "func.h"

/****** methods of param_list ******/

void param_list_init(param_list* paralst) {
    paralst->head = NULL;
    paralst->tail = NULL;
}

void param_list_add(param_list* paralst, char* type) {
    param_list_node* create = (param_list_node*)mem_alloc(sizeof(param_list_node));
    create->type = type;
    create->next = NULL;
    if (paralst->head != NULL) {
        paralst->tail->next = create;
        paralst->tail       = create;
    }
    else {
        paralst->head = create;
        paralst->tail = create;
    }
}

void param_list_destroy(param_list* paralst) {
    paralst->tail = paralst->head;
    param_list_node* temp;
    for (;;) {
        if (paralst->tail == NULL) {
            paralst->head =  NULL;
            return;
        }
        temp = paralst->tail;
        mem_free(temp);
        paralst->tail = paralst->tail->next;
    }
}

/****** methods of func ******/

void func_init(func* fn) {
    fn->func_access  = FUNC_ACCESS_OUT;
    fn->func_name    = NULL;
    param_list_init(&fn->func_params);
    param_list_init(&fn->func_retval);
}

void func_destroy(func* fn) {
    fn->func_name = NULL;
    param_list_destroy(&fn->func_params);
    param_list_destroy(&fn->func_retval);
}

/****** methods of func_table ******/

void func_table_init(func_table* functab) {
    functab->root = NULL;
}

// the algorithm is the same with type_table_cmp in src/type.c
static int func_table_cmp(char* name1, char* name2) {
    int64 i;
    for (i = 0; ; i++) {
        if (name1[i] < name2[i]) {
            return NODE_CMP_LT;
        }
        if (name1[i] > name2[i]){
            return NODE_CMP_GT;
        }
        if (name1[i] == '\0' && name2[i] == '\0') {
            return NODE_CMP_EQ;
        }
        if (name1[i] == '\0' && name2[i] != '\0') {
            return NODE_CMP_LT;
        }
        if (name1[i] != '\0' && name2[i] == '\0'){
            return NODE_CMP_GT;
        }
    }
}

// example:
//     |                       |
//    node                   rchild
//   /    \      ----\      /      \
//  a   rchild   ----/    node      c
//     /      \          /    \
//    b        c        a      b
static error func_table_left_rotate(func_table* functab, func_table_node* node) {
    if (node == NULL) {
        return new_error("err: the node will be left rotated should not be NULL.");
    }
    if (node->rchild == NULL) {
        return new_error("err: the node will be left rotated should have the right child.");
    }

    if (node == functab->root) {
        functab->root = node->rchild;
        node->rchild->parent = NULL;
    }
    else {
        if (node == node->parent->lchild) {
            node->parent->lchild = node->rchild;
        }
        else {
            node->parent->rchild = node->rchild;
        }
        node->rchild->parent = node->parent;
    }
    node->parent = node->rchild;
    node->rchild = node->rchild->lchild;
    if (node->parent->lchild != NULL) {
        node->parent->lchild->parent = node;
    }
    node->parent->lchild = node;
}

// example:
//        |                |
//       node            lchild
//      /    \  ----\   /      \
//   lchild   c ----/  a      node
//  /      \                 /    \
// a        b               b      c
static error func_table_right_rotate(func_table* functab, func_table_node* node) {
    if (node == NULL) {
        return new_error("err: the node will be right rotated should not be NULL.");
    }
    if (node->lchild == NULL) {
        return new_error("err: the node will be right rotated should have the left child");
    }

    if (node == functab->root) {
        functab->root = node->lchild;
        node->lchild->parent = NULL;
    }
    else {
        if (node == node->parent->lchild) {
            node->parent->lchild = node->lchild;
        }
        else {
            node->parent->rchild = node->lchild;
        }
        node->lchild->parent = node->parent;
    }
    node->parent = node->lchild;
    node->lchild = node->lchild->rchild;
    if (node->parent->rchild != NULL) {
        node->parent->rchild->parent = node;
    }
    node->parent->rchild = node;
}

// fix the balance of the tree and try to keep the properties
// of the red black tree.
static void func_table_add_fixup(func_table* functab, func_table_node* added) {
    func_table_node* uncle = NULL;
    for (;;) {
        if (added->parent == NULL || added->parent->parent == NULL) {
            break;
        }

        if (added->color == NODE_COLOR_RED && added->parent->color == NODE_COLOR_RED) {
            // the uncle is black or NULL
            if (added->parent == added->parent->parent->lchild) {
                uncle = added->parent->parent->rchild;
                if (uncle == NULL || uncle->color == NODE_COLOR_BLACK) {
                    if (added == added->parent->rchild) {
                        added =  added->parent;
                        func_table_left_rotate(functab, added);
                    }
                    added->parent->color         = NODE_COLOR_BLACK;
                    added->parent->parent->color = NODE_COLOR_RED;
                    func_table_right_rotate(functab, added->parent->parent);
                    break;
                }
            }
            else {
                uncle = added->parent->parent->lchild;
                if (uncle == NULL || uncle->color == NODE_COLOR_BLACK) {
                    if (added == added->parent->lchild) {
                        added =  added->parent;
                        func_table_right_rotate(functab, added);
                    }
                    added->parent->color         = NODE_COLOR_BLACK;
                    added->parent->parent->color = NODE_COLOR_RED;
                    func_table_left_rotate(functab, added->parent->parent);
                    break;
                }
            }

            // the uncle is red
            uncle->color         = NODE_COLOR_BLACK;
            uncle->parent->color = NODE_COLOR_RED;
            added->parent->color = NODE_COLOR_BLACK;
            added = added->parent->parent;
        }
        else {
            break;
        }
    }
    functab->root->color = NODE_COLOR_BLACK;
}

// note:
//   the funcinfo should be initialized with func_init()
//   before being assigned and being passed into the
//   func_table_add().
error func_table_add(func_table* functab, func funcinfo) {
    if (funcinfo.func_name == NULL) {
        return new_error("err: the function's name can not be NULL.");
    }
    func_table_node* create = (func_table_node*)mem_alloc(sizeof(func_table_node));
    create->funcinfo = funcinfo;
    create->color    = NODE_COLOR_RED;
    create->parent   = NULL;
    create->lchild   = NULL;
    create->rchild   = NULL;
    if (functab->root != NULL) {
        func_table_node* ptr = functab->root;
        for (;;) {
            switch (func_table_cmp(funcinfo.func_name, ptr->funcinfo.func_name)) {
            case NODE_CMP_LT:
                if (ptr->lchild == NULL) {
                    ptr->lchild = create;
                    create->parent = ptr;
                    func_table_add_fixup(functab, create);
                    return NULL;
                }
                ptr = ptr->lchild;
                break;
            case NODE_CMP_GT:
                if (ptr->rchild == NULL) {
                    ptr->rchild = create;
                    create->parent = ptr;
                    func_table_add_fixup(functab, create);
                    return NULL;
                }
                ptr = ptr->rchild;
                break;
            case NODE_CMP_EQ:
                return new_error("err: function redefined.");
            default:
                return new_error("err: can not compare the two function names.");
            }
        }
    }
    else {
        create->color = NODE_COLOR_BLACK;
        functab->root = create;
        return NULL;
    }
}

// return:
//       NULL -> not found the detail of the function
//   NOT NULL -> get the detail of the specific function
func* func_table_search(func_table* functab, char* func_name) {
    if (functab->root) {
        return NULL;
    }
    func_table_node* ptr = functab->root;
    for (;;) {
        switch (func_table_cmp(func_name, ptr->funcinfo.func_name)) {
        case NODE_CMP_LT:
            if (ptr->lchild != NULL) {
                ptr = ptr->lchild;
            }
            else {
                return NULL;
            }
            break;
        case NODE_CMP_GT:
            if (ptr->rchild != NULL) {
                ptr = ptr->rchild;
            }
            else {
                return NULL;
            }
            break;
        case NODE_CMP_EQ:
            return &ptr->funcinfo;
        default:
            return NULL;
        }
    }
}

static void func_table_destroy_node(func_table_node* node) {
    if (node != NULL) {
        if (node->lchild != NULL) func_table_destroy_node(node->lchild);
        if (node->rchild != NULL) func_table_destroy_node(node->rchild);
        func_destroy(&node->funcinfo);
        mem_free(node);
    }
}

void func_table_destroy(func_table* functab) {
    func_table_destroy_node(functab->root);
}
