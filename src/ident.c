/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "ident.h"

ident* make_id_const(char* id_name, int8 access, ident_type* constant_type, char* constant_value) {
    ident* id = (ident*)mem_alloc(sizeof(ident));
    id->id_name = id_name;
    id->access  = access;
    id->id_type = ID_CONST;
    id->id_info.id_const = (ident_const*)mem_alloc(sizeof(ident_const));
    id->id_info.id_const->const_type  = constant_type;
    id->id_info.id_const->const_value = constant_value;
    return id;
}

ident* make_id_var(char* id_name, int8 access, ident_type* variable_type, char* variable_value) {
    ident* id = (ident*)mem_alloc(sizeof(ident));
    id->id_name = id_name;
    id->access  = access;
    id->id_type = ID_VAR;
    id->id_info.id_var = (ident_var*)mem_alloc(sizeof(ident_var));
    id->id_info.id_var->var_type  = variable_type;
    id->id_info.id_var->var_value = variable_value;
    return id;
}

ident* make_id_type(char* id_name, int8 access) {
    ident* id = (ident*)mem_alloc(sizeof(ident));
    id->id_name = id_name;
    id->access  = access;
    id->id_type = ID_TYPE;
    id->id_info.id_type = (ident_type*)mem_alloc(sizeof(ident_type));
    id->id_info.id_type->itab = (ident_table*)mem_alloc(sizeof(ident_table));
    ident_table_init(id->id_info.id_type->itab);
    return id;
}

ident* make_id_func(char* id_name, int8 access) {
    ident* id = (ident*)mem_alloc(sizeof(ident));
    id->id_name = id_name;
    id->access  = access;
    id->id_type = ID_FUNC;
    id->id_info.id_func = (ident_func*)mem_alloc(sizeof(ident_func));
    id->id_info.id_func->passin.head = NULL;
    id->id_info.id_func->retout.head = NULL;
    return id;
}

ident* make_id_include(char* id_name) {
    ident* id = (ident*)mem_alloc(sizeof(ident));
    id->id_name = id_name;
    id->access  = ACCESS_NULL;
    id->id_type = ID_INCLUDE;
    id->id_info.id_include = (ident_include*)mem_alloc(sizeof(ident_include));
    id->id_info.id_include->itab = (ident_table*)mem_alloc(sizeof(ident_table));
    ident_table_init(id->id_info.id_include->itab);
    return id;
}

ident* make_id_module(char* id_name) {
    ident* id = (ident*)mem_alloc(sizeof(ident));
    id->id_name = id_name;
    id->access  = ACCESS_NULL;
    id->id_type = ID_MODULE;
    id->id_info.id_module = (ident_module*)mem_alloc(sizeof(ident_module));
    id->id_info.id_module->itab = (ident_table*)mem_alloc(sizeof(ident_table));
    ident_table_init(id->id_info.id_module->itab);
    return id;
}

/****** methods of ident_table ******/

void ident_table_init(ident_table* itab) {
    itab->root = NULL;
}

// firstly compare every character of the two name from index 0:
// (1) if name1[index] < name2[index] -> NODE_CMP_LT.
// (2) if name1[index] > name2[index] -> NODE_CMP_GT.
//
// if all characters compared are the same, then compare their names' length:
// (1) if the name1's length is longer , return NODE_CMP_GT.
// (2) if the name1's length is shorter, return NODE_CMP_LT.
// (3) if tow names'  length are equal , return NODE_CMP_EQ.
static int ident_table_cmp(char* name1, char* name2) {
    int64 i;
    for (i = 0; ; i++) {
        if (name1[i] < name2[i]) {
            return NODE_CMP_LT;
        }
        if (name1[i] > name2[i]) {
            return NODE_CMP_GT;
        }
        if (name1[i] == '\0' && name2[i] == '\0') {
            return NODE_CMP_EQ;
        }
        if (name1[i] == '\0' && name2[i] != '\0') {
            return NODE_CMP_LT;
        }
        if (name1[i] != '\0' && name2[i] == '\0') {
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
static error ident_table_left_rotate(ident_table* itab, ident_table_node* node) {
    if (node == itab->root) {
        itab->root = node->rchild;
        node->rchild->parent = NULL;
    }
    else {
        node == node->parent->lchild ? (node->parent->lchild = node->rchild) : (node->parent->rchild = node->rchild);
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
static error ident_table_right_rotate(ident_table* itab, ident_table_node* node) {
    if (node == itab->root) {
        itab->root = node->lchild;
        node->lchild->parent = NULL;
    }
    else {
        node == node->parent->lchild ? (node->parent->lchild = node->lchild) : (node->parent->rchild = node->lchild);
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
static void ident_table_add_fixup(ident_table* itab, ident_table_node* added) {
    ident_table_node* uncle = NULL;
    for (;;) {
        if (added->parent == NULL || added->parent->parent == NULL)
            break;

        if (added->color == NODE_COLOR_RED && added->parent->color == NODE_COLOR_RED) {
            // the uncle is black or NULL
            if (added->parent == added->parent->parent->lchild) {
                uncle = added->parent->parent->rchild;
                if (uncle == NULL || uncle->color == NODE_COLOR_BLACK) {
                    if (added == added->parent->rchild) {
                        added =  added->parent;
                        ident_table_left_rotate(itab, added);
                    }
                    added->parent->color         = NODE_COLOR_BLACK;
                    added->parent->parent->color = NODE_COLOR_RED;
                    ident_table_right_rotate(itab, added->parent->parent);
                    break;
                }
            }
            else {
                uncle = added->parent->parent->lchild;
                if (uncle == NULL || uncle->color == NODE_COLOR_BLACK) {
                    if (added == added->parent->lchild) {
                        added =  added->parent;
                        ident_table_right_rotate(itab, added);
                    }
                    added->parent->color         = NODE_COLOR_BLACK;
                    added->parent->parent->color = NODE_COLOR_RED;
                    ident_table_left_rotate(itab, added->parent->parent);
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
    itab->root->color = NODE_COLOR_BLACK;
}

error ident_table_add(ident_table* itab, ident* id) {
    if (id->id_name == NULL) {
        return new_error("the identifier's name can not be NULL.");
    }
    ident_table_node* create = (ident_table_node*)mem_alloc(sizeof(ident_table_node));
    create->id       = id;
    create->color    = NODE_COLOR_RED;
    create->parent   = NULL;
    create->lchild   = NULL;
    create->rchild   = NULL;
    if (itab->root != NULL) {
        ident_table_node* ptr = itab->root;
        for (;;) {
            switch (ident_table_cmp(id->id_name, ptr->id->id_name)) {
            case NODE_CMP_LT:
                if (ptr->lchild == NULL) {
                    ptr->lchild = create;
                    create->parent = ptr;
                    ident_table_add_fixup(itab, create);
                    return NULL;
                }
                ptr = ptr->lchild;
                break;
            case NODE_CMP_GT:
                if (ptr->rchild == NULL) {
                    ptr->rchild = create;
                    create->parent = ptr;
                    ident_table_add_fixup(itab, create);
                    return NULL;
                }
                ptr = ptr->rchild;
                break;
            case NODE_CMP_EQ:
                return new_error("err: type redefined.");
            default:
                return new_error("err: can not compare the two type names.");
            }
        }
    }
    else {
        create->color = NODE_COLOR_BLACK;
        itab->root = create;
        return NULL;
    }
}

// return:
//       NULL -> the identifier is not in the table.
//   NOT NULL -> the identifier is found.
ident* ident_table_search(ident_table* itab, char* id_name) {
    if (itab->root == NULL) {
        return NULL;
    }
    ident_table_node* ptr = itab->root;
    for (;;) {
        switch (ident_table_cmp(id_name, ptr->id->id_name)) {
        case NODE_CMP_LT:
            if (ptr->lchild != NULL)
                ptr = ptr->lchild;
            else
                return NULL;
            break;
        case NODE_CMP_GT:
            if (ptr->rchild != NULL)
                ptr = ptr->rchild;
            else
                return NULL;
            break;
        case NODE_CMP_EQ:
            return ptr->id;

        default:
            return NULL;
        }
    }
}

static void ident_table_destroy_node(ident_table_node* node) {
    if (node != NULL) {
        if (node->lchild != NULL) ident_table_destroy_node(node->lchild);
        if (node->rchild != NULL) ident_table_destroy_node(node->rchild);
        switch (node->id->id_type) {
        case ID_TYPE:
            ident_table_destroy(node->id->id_info.id_type->itab);
            break;
        case ID_FUNC: {
                param_list_node* del = NULL;
                param_list_node* ptr = node->id->id_info.id_func->passin.head;
                while (ptr != NULL) {
                    del = ptr;
                    ptr = ptr->next;
                    mem_free(del);
                }
                for (ptr = node->id->id_info.id_func->retout.head; ptr != NULL;) {
                    del = ptr;
                    ptr = ptr->next;
                    mem_free(del);
                }
                node->id->id_info.id_func->passin.head = NULL;
                node->id->id_info.id_func->retout.head = NULL;
            }
            break;
        case ID_INCLUDE:
            ident_table_destroy(node->id->id_info.id_include->itab);
            break;
        case ID_MODULE:
            ident_table_destroy(node->id->id_info.id_module->itab);
            break;
        }
        mem_free(node->id);
        mem_free(node);
    }
}

void ident_table_destroy(ident_table* itab) {
    ident_table_destroy_node(itab->root);
}
