/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "errors.h"

void error_table_init(error_table* errtab) {
    errtab->root       = NULL;
    errtab->must_count = 0;
}

static int error_table_cmp(char* name1, char* name2) {
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
static error error_table_left_rotate(error_table* errtab, error_table_node* node) {
    if (node == NULL) {
        return new_error("err: the node will be left rotated should not be NULL.");
    }
    if (node->rchild == NULL) {
        return new_error("err: the node will be left rotated should have the right child.");
    }

    if (node == errtab->root) {
        errtab->root = node->rchild;
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
static error error_table_right_rotate(error_table* errtab, error_table_node* node) {
    if (node == NULL) {
        return new_error("err: the node will be right rotated should not be NULL.");
    }
    if (node->lchild == NULL) {
        return new_error("err: the node will be right rotated should have the left child");
    }

    if (node == errtab->root) {
        errtab->root = node->lchild;
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
static void error_table_add_fixup(error_table* errtab, error_table_node* added) {
    error_table_node* uncle = NULL;
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
                        error_table_left_rotate(errtab, added);
                    }
                    added->parent->color         = NODE_COLOR_BLACK;
                    added->parent->parent->color = NODE_COLOR_RED;
                    error_table_right_rotate(errtab, added->parent->parent);
                    break;
                }
            }
            else {
                uncle = added->parent->parent->lchild;
                if (uncle == NULL || uncle->color == NODE_COLOR_BLACK) {
                    if (added == added->parent->lchild) {
                        added =  added->parent;
                        error_table_right_rotate(errtab, added);
                    }
                    added->parent->color         = NODE_COLOR_BLACK;
                    added->parent->parent->color = NODE_COLOR_RED;
                    error_table_left_rotate(errtab, added->parent->parent);
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
    errtab->root->color = NODE_COLOR_BLACK;
}

error error_table_add(error_table* errtab, char* errtag, bool must) {
    if (errtag == NULL || (must != true && must != false)) {
        return new_error("err: invalid paramter.");
    }
    error_table_node* create = (error_table_node*)mem_alloc(sizeof(error_table_node));
    create->errtag   = errtag;
    create->must     = must;
    create->color    = NODE_COLOR_RED;
    create->parent   = NULL;
    create->lchild   = NULL;
    create->rchild   = NULL;
    if (must == true) {
        errtab->must_count++;
    }
    if (errtab->root != NULL) {
        error_table_node* ptr = errtab->root;
        for (;;) {
            switch (error_table_cmp(errtag, ptr->errtag)) {
            case NODE_CMP_LT:
                if (ptr->lchild == NULL) {
                    ptr->lchild = create;
                    create->parent = ptr;
                    error_table_add_fixup(errtab, create);
                    return NULL;
                }
                ptr = ptr->lchild;
                break;
            case NODE_CMP_GT:
                if (ptr->rchild == NULL) {
                    ptr->rchild = create;
                    create->parent = ptr;
                    error_table_add_fixup(errtab, create);
                    return NULL;
                }
                ptr = ptr->rchild;
                break;
            case NODE_CMP_EQ:
                // all same errors are inserted in the same node
                return NULL;
            default:
                return new_error("err: can not compare the two type names.");
            }
        }
    }
    else {
        create->color = NODE_COLOR_BLACK;
        errtab->root = create;
        return NULL;
    }
}

// return:
//   true  -> the specific error happened and be found in the table.
//   false -> the specific error not happened.
bool error_table_match_full(error_table* errtab, char* errtag) {
    if (errtab->root == NULL) {
        return false;
    }
    error_table_node* ptr = errtab->root;
    for (;;) {
        switch (error_table_cmp(errtag, ptr->errtag)) {
        case NODE_CMP_LT:
            if (ptr->lchild != NULL) {
                ptr = ptr->lchild;
            }
            else {
                return false;
            }
            break;
        case NODE_CMP_GT:
            if (ptr->rchild != NULL) {
                ptr = ptr->rchild;
            }
            else {
                return false;
            }
            break;
        case NODE_CMP_EQ:
            // TODO: delete the node from the table
            return true;
        default:
            return false;
        }
    }
}

bool error_table_match_wdcd(error_table* errtab, char* errtag) {
    if (errtab->root == NULL) {
        return false;
    }
    // TODO: continue to implement
}

bool error_table_exist_must(error_table* errtab) {
    if (errtab->must_count > 0) {
        return true;
    }
    return false;
}

static void error_table_destroy_node(error_table_node* node) {
    if (node != NULL) {
        if (node->lchild != NULL) error_table_destroy_node(node->lchild);
        if (node->rchild != NULL) error_table_destroy_node(node->rchild);
        mem_free(node);
    }
}

void error_table_destroy(error_table* errtab) {
    error_table_destroy_node(errtab->root);
}
