/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "type.h"

/****** methods of type ******/

void type_init(type* typ) {
    typ->type_access  = TYPE_ACCESS_OUT;
    typ->type_name    = NULL;
    decl_list_init (&typ->type_properties);
    func_table_init(&typ->type_methods);
}

void type_destroy(type* typ) {
    decl_list_destroy (&typ->type_properties);
    func_table_destroy(&typ->type_methods);
}

/****** methods of type_table ******/

void type_table_init(type_table* typetab) {
    typetab->root = NULL;
}

static int type_table_cmp(char* name1, char* name2) {
    // compare every character of the two typename from index 0.
    // if t1's typename[index] < t2's typename[index] -> NODE_CMP_LT.
    // if t1's typename[index] > t2's typename[index] -> NODE_CMP_GT.
    // if all characters compared are the same, then compare their
    // typenames' length. if the t1's length is longer, return
    // NODE_CMP_GT, if the t1's length is equal to the t2's length,
    // then return NODE_CMP_EQ.
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
static error type_table_left_rotate(type_table* typetab, type_table_node* node) {
    if (node == NULL) {
        return new_error("err: the node will be left rotated should not be NULL.");
    }
    if (node->rchild == NULL) {
        return new_error("err: the node will be left rotated should have the right child.");
    }

    if (node == typetab->root) {
        typetab->root = node->rchild;
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
static error type_table_right_rotate(type_table* typetab, type_table_node* node) {
    if (node == NULL) {
        return new_error("err: the node will be right rotated should not be NULL.");
    }
    if (node->lchild == NULL) {
        return new_error("err: the node will be right rotated should have the left child");
    }

    if (node == typetab->root) {
        typetab->root = node->lchild;
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
static void type_table_add_fixup(type_table* typetab, type_table_node* added) {
    type_table_node* uncle = NULL;
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
                        type_table_left_rotate(typetab, added);
                    }
                    added->parent->color         = NODE_COLOR_BLACK;
                    added->parent->parent->color = NODE_COLOR_RED;
                    type_table_right_rotate(typetab, added->parent->parent);
                    break;
                }
            }
            else {
                uncle = added->parent->parent->lchild;
                if (uncle == NULL || uncle->color == NODE_COLOR_BLACK) {
                    if (added == added->parent->lchild) {
                        added =  added->parent;
                        type_table_right_rotate(typetab, added);
                    }
                    added->parent->color         = NODE_COLOR_BLACK;
                    added->parent->parent->color = NODE_COLOR_RED;
                    type_table_left_rotate(typetab, added->parent->parent);
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
    typetab->root->color = NODE_COLOR_BLACK;
}

// note:
//   the typeinfo should be initialized with type_init()
//   before being assigned and being passed into the
//   type_table_add().
error type_table_add(type_table* typetab, type typeinfo) {
    if (typeinfo.type_name == NULL) {
        return new_error("err: the type's name can not be NULL.");
    }
    type_table_node* create = (type_table_node*)mem_alloc(sizeof(type_table_node));
    create->typeinfo = typeinfo;
    create->color    = NODE_COLOR_RED;
    create->parent   = NULL;
    create->lchild   = NULL;
    create->rchild   = NULL;
    if (typetab->root != NULL) {
        type_table_node* ptr = typetab->root;
        for (;;) {
            switch (type_table_cmp(typeinfo.type_name, ptr->typeinfo.type_name)) {
            case NODE_CMP_LT:
                if (ptr->lchild == NULL) {
                    ptr->lchild = create;
                    create->parent = ptr;
                    type_table_add_fixup(typetab, create);
                    return NULL;
                }
                ptr = ptr->lchild;
                break;
            case NODE_CMP_GT:
                if (ptr->rchild == NULL) {
                    ptr->rchild = create;
                    create->parent = ptr;
                    type_table_add_fixup(typetab, create);
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
        typetab->root = create;
        return NULL;
    }
}

// return:
//       NULL -> not found the detail of the type
//   NOT NULL -> get the detail of the specific type
type* type_table_search(type_table* typetab, char* type_name) {
    if (typetab->root == NULL) {
        return NULL;
    }
    type_table_node* ptr = typetab->root;
    for (;;) {
        switch (type_table_cmp(type_name, ptr->typeinfo.type_name)) {
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
            return &ptr->typeinfo;
        default:
            return NULL;
        }
    }
}

static void type_table_destroy_node(type_table_node* node) {
    if (node != NULL) {
        if (node->lchild != NULL) type_table_destroy_node(node->lchild);
        if (node->rchild != NULL) type_table_destroy_node(node->rchild);
        type_destroy(&node->typeinfo);
        mem_free(node);
    }
}

void type_table_destroy(type_table* typetab) {
    type_table_destroy_node(typetab->root);
}
