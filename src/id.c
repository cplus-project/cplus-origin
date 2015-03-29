/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "id.h"

void id_table_init(id_table* idtab) {
    idtab->root = NULL;
}

static int id_table_cmp(char* name1, char* name2) {
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
static error id_table_left_rotate(id_table* idtab, id_table_node* node) {
    if (node == NULL) {
        return new_error("err: the node will be left rotated should not be NULL.");
    }
    if (node->rchild == NULL) {
        return new_error("err: the node will be left rotated should have the right child.");
    }

    if (node == idtab->root) {
        idtab->root = node->rchild;
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
static error id_table_right_rotate(id_table* idtab, id_table_node* node) {
    if (node == NULL) {
        return new_error("err: the node will be right rotated should not be NULL.");
    }
    if (node->lchild == NULL) {
        return new_error("err: the node will be right rotated should have the left child");
    }

    if (node == idtab->root) {
        idtab->root = node->lchild;
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
static void id_table_add_fixup(id_table* idtab, id_table_node* added) {
    id_table_node* uncle = NULL;
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
                        id_table_left_rotate(idtab, added);
                    }
                    added->parent->color         = NODE_COLOR_BLACK;
                    added->parent->parent->color = NODE_COLOR_RED;
                    id_table_right_rotate(idtab, added->parent->parent);
                    break;
                }
            }
            else {
                uncle = added->parent->parent->lchild;
                if (uncle == NULL || uncle->color == NODE_COLOR_BLACK) {
                    if (added == added->parent->lchild) {
                        added =  added->parent;
                        id_table_right_rotate(idtab, added);
                    }
                    added->parent->color         = NODE_COLOR_BLACK;
                    added->parent->parent->color = NODE_COLOR_RED;
                    id_table_left_rotate(idtab, added->parent->parent);
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
    idtab->root->color = NODE_COLOR_BLACK;
}

error id_table_add(id_table* idtab, id idinfo) {
    if (idinfo.id_name == NULL) {
        return new_error("err: the id name can not be NULL.");
    }
    id_table_node* create = (id_table_node*)mem_alloc(sizeof(id_table_node));
    create->idinfo   = idinfo;
    create->color    = NODE_COLOR_RED;
    create->parent   = NULL;
    create->lchild   = NULL;
    create->rchild   = NULL;
    if (idtab->root != NULL) {
        id_table_node* ptr = idtab->root;
        for (;;) {
            switch (id_table_cmp(idinfo.id_name, ptr->idinfo.id_name)) {
            case NODE_CMP_LT:
                if (ptr->lchild == NULL) {
                    ptr->lchild = create;
                    create->parent = ptr;
                    id_table_add_fixup(idtab, create);
                    return NULL;
                }
                ptr = ptr->lchild;
                break;
            case NODE_CMP_GT:
                if (ptr->rchild == NULL) {
                    ptr->rchild = create;
                    create->parent = ptr;
                    id_table_add_fixup(idtab, create);
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
        idtab->root = create;
        return NULL;
    }
}

// node:
//   you shouldn't update an id through the id_table_update() if
//   that id will be updated many times, because this function
//   will always do an extra search operation.
//
//   I suggest you first call the function id_table_search() to
//   get the pointer of the id you want to modify, and then you
//   can modify it more efficiently.
error id_table_update(id_table* idtab, id idinfo) {
    if (idtab->root == NULL) {
        return new_error("err: the id table is empty.");
    }
    id_table_node* ptr = idtab->root;
    for (;;) {
        switch (id_table_cmp(idinfo.id_name, ptr->idinfo.id_name)) {
        case NODE_CMP_LT:
            if (ptr->lchild != NULL) {
                ptr = ptr->lchild;
            }
            else {
                return new_error("err: not found the specific id and can not update it.");
            }
            break;
        case NODE_CMP_GT:
            if (ptr->rchild != NULL) {
                ptr = ptr->rchild;
            }
            else {
                return new_error("err: not found the specific id and can not update it.");
            }
            break;
        case NODE_CMP_EQ:
            ptr->idinfo = idinfo;
        default:
            return new_error("err: compare error.");
        }
    }
}

// return:
//       NULL -> not found the detail of the id
//   NOT NULL -> get the detail of the specific id
id* id_table_search(id_table* idtab, char* id_name) {
    if (idtab->root == NULL) {
        return NULL;
    }
    id_table_node* ptr = idtab->root;
    for (;;) {
        switch (id_table_cmp(id_name, ptr->idinfo.id_name)) {
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
            return &ptr->idinfo;
        default:
            return NULL;
        }
    }
}

static void id_table_destroy_node(id_table_node* node) {
    if (node != NULL) {
        if (node->lchild != NULL) id_table_destroy_node(node->lchild);
        if (node->rchild != NULL) id_table_destroy_node(node->rchild);
        mem_free(node);
    }
}

void id_table_destroy(id_table* idtab) {
    id_table_destroy_node(idtab->root);
}
