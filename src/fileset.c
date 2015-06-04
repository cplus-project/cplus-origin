/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "fileset.h"

/****** methods of FileQueue ******/

void fileQueueInit(FileQueue* fqueue) {
    fqueue->head = NULL;
    fqueue->tail = NULL;
}

void fileQueueEnqueue(FileQueue* fqueue, char* file_name) {
    FileQueueNode* create = (FileQueueNode*)mem_alloc(sizeof(FileQueueNode));
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
bool fileQueueExist(FileQueue* fqueue, char* file_name) {
    int i;
    FileQueueNode* ptr;
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
char* fileQueueFront(FileQueue* fqueue) {
    if (fqueue->head == NULL) {
        return NULL;
    }
    return fqueue->head->file_name;
}

error fileQueueDequeue(FileQueue* fqueue) {
    if (fqueue->head == NULL) {
        return new_error("the file queue is empty.");
    }
    FileQueueNode* temp = fqueue->head;
    fqueue->head = fqueue->head->next;
    if (fqueue->head == NULL) {
        fqueue->tail =  NULL;
    }
    temp->next = NULL;
    mem_free(temp);
}

void fileQueueDestroy(FileQueue* fqueue) {
    FileQueueNode* temp;
    for (;;) {
        if (fqueue->head == NULL) {
            fqueue->tail =  NULL;
        }
        temp = fqueue->head;
        fqueue->head = fqueue->head->next;
        mem_free(temp);
    }
}

/****** methods of FileStack ******/

void fileStackInit(FileStack* fstk) {
    fstk->top = NULL;
}

void fileStackPush(FileStack* fstk, char* file_name) {
    FileStackNode* create = (FileStackNode*)mem_alloc(sizeof(FileStackNode));
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
bool fileStackIsEmpty(FileStack* fstk) {
    if (fstk->top != NULL) {
        return false;
    }
    return true;
}

char* fileStackTop(FileStack* fstk) {
    return fstk->top->file_name;
}

void fileStackPop(FileStack* fstk) {
    FileStackNode* temp = fstk->top;
    fstk->top = fstk->top->next;
    mem_free(temp);
}

void fileStackDestroy(FileStack* fstk) {
    FileStackNode* temp;
    for (;;) {
        if (fstk->top == NULL) {
            return;
        }
        temp = fstk->top;
        fstk->top = fstk->top->next;
        mem_free(temp);
    }
}

/****** methods of FileTree ******/

void fileTreeInit(FileTree* ftree) {
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
static int fileTreeCmp(char* name1, char* name2) {
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
static error fileTreeLeftRotate(FileTree* ftree, FileTreeNode* node) {
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
static error fileTreeRightRotate(FileTree* ftree, FileTreeNode* node) {
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
static void fileTreeAddFixup(FileTree* ftree, FileTreeNode* added) {
    FileTreeNode* uncle = NULL;
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
                        fileTreeLeftRotate(ftree, added);
                    }
                    added->parent->color         = NODE_COLOR_BLACK;
                    added->parent->parent->color = NODE_COLOR_RED;
                    fileTreeRightRotate(ftree, added->parent->parent);
                    break;
                }
            }
            else {
                uncle = added->parent->parent->lchild;
                if (uncle == NULL || uncle->color == NODE_COLOR_BLACK) {
                    if (added == added->parent->lchild) {
                        added =  added->parent;
                        fileTreeRightRotate(ftree, added);
                    }
                    added->parent->color         = NODE_COLOR_BLACK;
                    added->parent->parent->color = NODE_COLOR_RED;
                    fileTreeLeftRotate(ftree, added->parent->parent);
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

error fileTreeAdd(FileTree* ftree, char* file_name) {
    if (file_name == NULL) {
        return new_error("the filename can not be NULL.");
    }
    FileTreeNode* create = (FileTreeNode*)mem_alloc(sizeof(FileTreeNode));
    create->file_name = file_name;
    create->color     = NODE_COLOR_RED;
    create->parent    = NULL;
    create->lchild    = NULL;
    create->rchild    = NULL;
    if (ftree->root != NULL) {
        FileTreeNode* ptr = ftree->root;
        for (;;) {
            switch (fileTreeCmp(file_name, ptr->file_name)) {
            case NODE_CMP_LT:
                if (ptr->lchild == NULL) {
                    ptr->lchild = create;
                    create->parent = ptr;
                    fileTreeAddFixup(ftree, create);
                    return NULL;
                }
                ptr = ptr->lchild;
                break;
            case NODE_CMP_GT:
                if (ptr->rchild == NULL) {
                    ptr->rchild = create;
                    create->parent = ptr;
                    fileTreeAddFixup(ftree, create);
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
//    true  -> the file exists.
//    false -> the file not exists.
bool fileTreeExist(FileTree* ftree, char* file_name) {
    if (ftree->root == NULL) {
        return false;
    }
    FileTreeNode* ptr = ftree->root;
    for (;;) {
        switch (fileTreeCmp(file_name, ptr->file_name)) {
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

static void fileTreeDestroyNode(FileTreeNode* node) {
    if (node != NULL) {
        if (node->lchild != NULL) fileTreeDestroyNode(node->lchild);
        if (node->rchild != NULL) fileTreeDestroyNode(node->rchild);
        mem_free(node);
    }
}

void fileTreeDestroy(FileTree* ftree) {
    fileTreeDestroyNode(ftree->root);
}
