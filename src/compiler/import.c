/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "import.h"

/****** methods of CompileWaitQueue ******/

void compileWaitQueueInit(CompileWaitQueue* waitqueue) {
    waitqueue->cur  = NULL;
    waitqueue->head = (CompileWaitQueueNode*)mem_alloc(sizeof(CompileWaitQueueNode));
    waitqueue->head->next = NULL;
    waitqueue->head->prev = NULL;
}

// return true if the queue is empty.
bool compileWaitQueueIsEmpty(CompileWaitQueue* waitqueue) {
    return waitqueue->head->next == NULL ? true : false;
}

void compileWaitQueueEnqueue(CompileWaitQueue* waitqueue, char* file_name) {
    CompileWaitQueueNode* create = (CompileWaitQueueNode*)mem_alloc(sizeof(CompileWaitQueueNode));
    create->file_info.file_name     = file_name;
    create->file_info.depend_parsed = false;

    if (waitqueue->head->next != NULL) {
        create->prev = waitqueue->cur->prev;
        create->next = waitqueue->cur;
        waitqueue->cur->prev->next = create;
        waitqueue->cur->prev = create;
    }
    else {
        create->next = NULL;
        create->prev = waitqueue->head;
        waitqueue->head->next = create;
    }
}

// return the head node of the queue and set the head node to the
// 'cur' position. all new dependency nodes will be inserted into
// the previous position of the 'cur' position.
//
WaitCompileFile* compileWaitQueueGetFile(CompileWaitQueue* waitqueue) {
    waitqueue->cur = waitqueue->head->next;
    return waitqueue->cur != NULL ? &(waitqueue->cur->file_info) : NULL;
}

void compileWaitQueueDequeue(CompileWaitQueue* waitqueue) {
    CompileWaitQueueNode* del = waitqueue->head->next;
    if (del != NULL) {
        if (del->next != NULL) {
            waitqueue->head->next = del->next;
            del->next->prev = waitqueue->head;
            del->prev = NULL;
            del->next = NULL;
        }
        else {
            waitqueue->head->next = NULL;
            del->prev = NULL;
        }
        mem_free(del);
    }
}

void compileWaitQueueDestroy(CompileWaitQueue* waitqueue) {
    CompileWaitQueueNode* del = NULL;
    CompileWaitQueueNode* ptr = waitqueue->head;
    for (;;) {
        if (ptr == NULL) {
            waitqueue->head = NULL;
            waitqueue->cur  = NULL;
            return;
        }
        del = ptr;
        ptr = ptr->next;

        del->next = NULL;
        del->prev = NULL;
        mem_free(del);
    }
}

/****** methods of CompileCacheTree ******/

void compileCacheTreeInit(CompileCacheTree* cachetree) {
    cachetree->root = NULL;
}

// firstly compare every character of the two name from index 0:
// (1) if name1[index] < name2[index] -> NODE_CMP_LT.
// (2) if name1[index] > name2[index] -> NODE_CMP_GT.
//
// if all characters compared are the same, then compare their names' length:
// (1) if the name1's length is longer , return NODE_CMP_GT.
// (2) if the name1's length is shorter, return NODE_CMP_LT.
// (3) if tow names'  length are equal , return NODE_CMP_EQ.
static int compileCacheTreeCmp(char* name1, char* name2) {
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
static error compileCacheTreeLeftRotate(CompileCacheTree* cachetree, CompileCacheTreeNode* node) {
    if (node == cachetree->root) {
        cachetree->root = node->rchild;
        node->rchild->parent = NULL;
    }
    else {
        (node == node->parent->lchild)?
        (node->parent->lchild = node->rchild):
        (node->parent->rchild = node->rchild);
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
static error compileCacheTreeRightRotate(CompileCacheTree* cachetree, CompileCacheTreeNode* node) {
    if (node == cachetree->root) {
        cachetree->root = node->lchild;
        node->lchild->parent = NULL;
    }
    else {
        (node == node->parent->lchild)?
        (node->parent->lchild = node->lchild):
        (node->parent->rchild = node->lchild);
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
static void compileCacheTreeAddFixup(CompileCacheTree* cachetree, CompileCacheTreeNode* added) {
    CompileCacheTreeNode* uncle = NULL;
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
                        compileCacheTreeLeftRotate(cachetree, added);
                    }
                    added->parent->color         = NODE_COLOR_BLACK;
                    added->parent->parent->color = NODE_COLOR_RED;
                    compileCacheTreeRightRotate(cachetree, added->parent->parent);
                    break;
                }
            }
            else {
                uncle = added->parent->parent->lchild;
                if (uncle == NULL || uncle->color == NODE_COLOR_BLACK) {
                    if (added == added->parent->lchild) {
                        added =  added->parent;
                        compileCacheTreeRightRotate(cachetree, added);
                    }
                    added->parent->color         = NODE_COLOR_BLACK;
                    added->parent->parent->color = NODE_COLOR_RED;
                    compileCacheTreeLeftRotate(cachetree, added->parent->parent);
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
    cachetree->root->color = NODE_COLOR_BLACK;
}

// used to verify whether a file entry is in the cache tree no matter has it been
// compiled already.
//
// return:
//    true  -> the file entry is already in the cache tree
//    false -> the file entry is not in the cache tree
//
bool compileCacheTreeExist(CompileCacheTree* cachetree, char* file_name) {
    if (cachetree->root == NULL) {
        return false;
    }
    CompileCacheTreeNode* ptr = cachetree->root;
    for (;;) {
        switch (compileCacheTreeCmp(file_name, ptr->file_name)) {
        case NODE_CMP_LT:
            if (ptr->lchild != NULL) {
                ptr = ptr->lchild;
                break;
            }
            return false;

        case NODE_CMP_GT:
            if (ptr->rchild != NULL) {
                ptr = ptr->rchild;
                break;
            }
            return false;

        case NODE_CMP_EQ:
            return true;
        }
    }
}

// just create a new file cache entry in the cache tree. the new entry's identifier
// table is set to NULL.
//
error compileCacheTreeCacheNew(CompileCacheTree* cachetree, char* file_name) {
    if (file_name == NULL) {
        return new_error("the file name can not be NULL.");
    }
    CompileCacheTreeNode* create = (CompileCacheTreeNode*)mem_alloc(sizeof(CompileCacheTreeNode));
    create->file_name = file_name;
    create->id_table  = NULL;
    create->color     = NODE_COLOR_RED;
    create->parent    = NULL;
    create->lchild    = NULL;
    create->rchild    = NULL;
    if (cachetree->root != NULL) {
        CompileCacheTreeNode* ptr = cachetree->root;
        for (;;) {
            switch (compileCacheTreeCmp(file_name, ptr->file_name)) {
            case NODE_CMP_LT:
                if (ptr->lchild == NULL) {
                    ptr->lchild = create;
                    create->parent = ptr;
                    compileCacheTreeAddFixup(cachetree, create);
                    return NULL;
                }
                ptr = ptr->lchild;
                break;

            case NODE_CMP_GT:
                if (ptr->rchild == NULL) {
                    ptr->rchild = create;
                    create->parent = ptr;
                    compileCacheTreeAddFixup(cachetree, create);
                    return NULL;
                }
                ptr = ptr->rchild;
                break;

            case NODE_CMP_EQ:
                return ptr->id_table == NULL ? new_error("include circle.") : NULL;
            }
        }
    }
    else {
        create->color = NODE_COLOR_BLACK;
        cachetree->root = create;
        return NULL;
    }
}

// when a file is compiled over and its identifier table is exported successfully,
// the identifier table should be assigned to the associate file entry in the cache
// tree.
//
error compileCacheTreeCacheOver(CompileCacheTree* cachetree, char* file_name, IdentTable* id_table) {
    if (cachetree->root == NULL) {
        return NULL;
    }
    CompileCacheTreeNode* ptr = cachetree->root;
    for (;;) {
        switch (compileCacheTreeCmp(file_name, ptr->file_name)) {
        case NODE_CMP_LT:
            if (ptr->lchild != NULL) {
                ptr = ptr->lchild;
                break;
            }
            return new_error("not found the file's cache entry in cache tree.");

        case NODE_CMP_GT:
            if (ptr->rchild != NULL) {
                ptr = ptr->rchild;
                break;
            }
            return new_error("not found the file's cache entry in cache tree.");

        case NODE_CMP_EQ:
            if (id_table != NULL) {
                ptr->id_table = id_table;
                return NULL;
            }
            return new_error("assign a NULL identifier table to the file cache entry.");
        }
    }
}

// get the file's id table. if the file doesn't have the entry in the cache tree or
// the file only has an entry but without compiled yet, a NULL will be returned.
//
// return:
//       NULL -> the file is not compiled over
//   NOT NULL -> the file is compiled over
//
IdentTable* compileCacheTreeCacheGet(CompileCacheTree* cachetree, char* file_name) {
    if (cachetree->root == NULL) {
        return NULL;
    }
    CompileCacheTreeNode* ptr = cachetree->root;
    for (;;) {
        switch (compileCacheTreeCmp(file_name, ptr->file_name)) {
        case NODE_CMP_LT:
            if (ptr->lchild != NULL) {
                ptr = ptr->lchild;
                break;
            }
            return NULL;

        case NODE_CMP_GT:
            if (ptr->rchild != NULL) {
                ptr = ptr->rchild;
                break;
            }
            return NULL;

        case NODE_CMP_EQ:
            return ptr->id_table;
        }
    }
}

static void compileCacheTreeDestroyNode(CompileCacheTreeNode* node) {
    if (node != NULL) {
        if (node->lchild != NULL) compileCacheTreeDestroyNode(node->lchild);
        if (node->rchild != NULL) compileCacheTreeDestroyNode(node->rchild);
        if (node->id_table != NULL) {
            identTableDestroy(node->id_table);
        }
        mem_free(node);
    }
}

void compileCacheTreeDestroy(CompileCacheTree* cachetree) {
    compileCacheTreeDestroyNode(cachetree->root);
}
