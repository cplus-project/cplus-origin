/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "fileset.h"

/****** methods of file_queue ******/

void file_queue_init(file_queue* fqueue) {
    fqueue->head = NULL;
    fqueue->tail = NULL;
}

void file_queue_enqueue(file_queue* fqueue, char* file_name) {
    file_queue_node* create = (file_queue_node*)mem_alloc(sizeof(file_queue_node));
    create->file_name = file_name;
    create->next      = NULL;
    if (fqueue->head != NULL) {
        fqueue->tail->next = create;
        fqueue->tail = create;
    }
    else {
        fqueue->head = create;
        fqueue->tail = create;
    }
}

// return true if the file is already in the queue.
bool file_queue_exist(file_queue* fqueue, char* file_name) {
    int i;
    file_queue_node* ptr;
    for (ptr = fqueue->head; ptr != NULL; ptr = ptr->next) {
        for (i = 0; ; i++) {
            if (file_name[i] != ptr->file_name[i]) {
                break;
            }
            if (file_name[i] == '\0' && ptr->file_name[i] == '\0') {
                return true;
            }
        }
    }
    return false;
}

// return NULL if the queue is empty.
char* file_queue_front(file_queue* fqueue) {
    if (fqueue->head == NULL) {
        return NULL;
    }
    return fqueue->head->file_name;
}

error file_queue_dequeue(file_queue* fqueue) {
    if (fqueue->head == NULL) {
        return new_error("the file queue is empty.");
    }
    file_queue_node* temp = fqueue->head;
    fqueue->head = fqueue->head->next;
    if (fqueue->head == NULL) {
        fqueue->tail =  NULL;
    }
    temp->next = NULL;
    mem_free(temp);
}

void file_queue_destroy(file_queue* fqueue) {
    file_queue_node* temp;
    for (;;) {
        if (fqueue->head == NULL) {
            fqueue->tail =  NULL;
        }
        temp = fqueue->head;
        fqueue->head = fqueue->head->next;
        mem_free(temp);
    }
}

/****** methods of file_stack ******/

void file_stack_init(file_stack* fstk) {
    fstk->top = NULL;
}

void file_stack_push(file_stack* fstk, char* file_name) {
    file_stack_node* create = (file_stack_node*)mem_alloc(sizeof(file_stack_node));
    create->file_name = file_name;
    create->next      = NULL;
    if (fstk->top != NULL) {
        create->next = fstk->top;
        fstk->top = create;
    }
    else {
        fstk->top = create;
    }
}

// return true if the stack is empty.
bool file_stack_isempty(file_stack* fstk) {
    if (fstk->top != NULL) {
        return false;
    }
    return true;
}

char* file_stack_top(file_stack* fstk) {
    return fstk->top->file_name;
}

void file_stack_pop(file_stack* fstk) {
    file_stack_node* temp = fstk->top;
    fstk->top = fstk->top->next;
    mem_free(temp);
}

void file_stack_destroy(file_stack* fstk) {
    file_stack_node* temp;
    for (;;) {
        if (fstk->top == NULL) {
            return;
        }
        temp = fstk->top;
        fstk->top = fstk->top->next;
        mem_free(temp);
    }
}

/****** methods of file_tree ******/

void file_tree_init(file_tree* ftree) {
    ftree->root = NULL;
}

// firstly compare every character of the two name from index 0:
// (1) if name1[index] < name2[index] -> NODE_CMP_LT.
// (2) if name1[index] > name2[index] -> NODE_CMP_GT.
//
// if all characters compared are the same, then compare their names' length:
// (1) if the name1's length is longer , return NODE_CMP_GT.
// (2) if the name1's length is shorter, return NODE_CMP_LT.
// (3) if tow names'  length are equal , return NODE_CMP_EQ.
static int file_tree_cmp(char* name1, char* name2) {
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
static error file_tree_left_rotate(file_tree* ftree, file_tree_node* node) {
    if (node == ftree->root) {
        ftree->root = node->rchild;
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
static error file_tree_right_rotate(file_tree* ftree, file_tree_node* node) {
    if (node == ftree->root) {
        ftree->root = node->lchild;
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
static void file_tree_add_fixup(file_tree* ftree, file_tree_node* added) {
    file_tree_node* uncle = NULL;
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
                        file_tree_left_rotate(ftree, added);
                    }
                    added->parent->color         = NODE_COLOR_BLACK;
                    added->parent->parent->color = NODE_COLOR_RED;
                    file_tree_right_rotate(ftree, added->parent->parent);
                    break;
                }
            }
            else {
                uncle = added->parent->parent->lchild;
                if (uncle == NULL || uncle->color == NODE_COLOR_BLACK) {
                    if (added == added->parent->lchild) {
                        added =  added->parent;
                        file_tree_right_rotate(ftree, added);
                    }
                    added->parent->color         = NODE_COLOR_BLACK;
                    added->parent->parent->color = NODE_COLOR_RED;
                    file_tree_left_rotate(ftree, added->parent->parent);
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
    ftree->root->color = NODE_COLOR_BLACK;
}

error file_tree_add(file_tree* ftree, char* file_name) {
    if (file_name == NULL) {
        return new_error("the filename can not be NULL.");
    }
    file_tree_node* create = (file_tree_node*)mem_alloc(sizeof(file_tree_node));
    create->file_name = file_name;
    create->color     = NODE_COLOR_RED;
    create->parent    = NULL;
    create->lchild    = NULL;
    create->rchild    = NULL;
    if (ftree->root != NULL) {
        file_tree_node* ptr = ftree->root;
        for (;;) {
            switch (file_tree_cmp(file_name, ptr->file_name)) {
            case NODE_CMP_LT:
                if (ptr->lchild == NULL) {
                    ptr->lchild = create;
                    create->parent = ptr;
                    file_tree_add_fixup(ftree, create);
                    return NULL;
                }
                ptr = ptr->lchild;
                break;
            case NODE_CMP_GT:
                if (ptr->rchild == NULL) {
                    ptr->rchild = create;
                    create->parent = ptr;
                    file_tree_add_fixup(ftree, create);
                    return NULL;
                }
                ptr = ptr->rchild;
                break;
            case NODE_CMP_EQ:
                return new_error("the file is already in the tree.");
            default:
                return new_error("err: can not compare the two filenames.");
            }
        }
    }
    else {
        create->color = NODE_COLOR_BLACK;
        ftree->root = create;
        return NULL;
    }
}

// return:
//    true  -> the file is found.
//    false -> the file is not in the table.
bool file_tree_exist(file_tree* ftree, char* file_name) {
    if (ftree->root == NULL) {
        return false;
    }
    file_tree_node* ptr = ftree->root;
    for (;;) {
        switch (file_tree_cmp(file_name, ptr->file_name)) {
        case NODE_CMP_LT:
            if (ptr->lchild != NULL)
                ptr = ptr->lchild;
            else
                return false;
            break;
        case NODE_CMP_GT:
            if (ptr->rchild != NULL)
                ptr = ptr->rchild;
            else
                return false;
            break;
        case NODE_CMP_EQ:
            return true;

        default:
            return false;
        }
    }
}

static void file_tree_destroy_node(file_tree_node* node) {
    if (node != NULL) {
        if (node->lchild != NULL) file_tree_destroy_node(node->lchild);
        if (node->rchild != NULL) file_tree_destroy_node(node->rchild);
        mem_free(node);
    }
}

void file_tree_destroy(file_tree* ftree) {
    file_tree_destroy_node(ftree->root);
}
