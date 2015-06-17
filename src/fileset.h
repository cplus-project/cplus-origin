/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The fileset.h and fileset.c provide some struct
 * to storage and organize a set of files.
 **/

#ifndef CPLUS_FILESET_H
#define CPLUS_FILESET_H

#include "common.h"

typedef struct FileQueueNode {
    char* file_name;
    struct FileQueueNode* next;
}FileQueueNode;

// a queue struct to storage files.
typedef struct {
    FileQueueNode* head;
    FileQueueNode* tail;
}FileQueue;

extern void  fileQueueInit   (FileQueue* fqueue);
extern void  fileQueueEnqueue(FileQueue* fqueue, char* file_name);
extern bool  fileQueueExist  (FileQueue* fqueue, char* file_name);
extern char* fileQueueFront  (FileQueue* fqueue);
extern error fileQueueDequeue(FileQueue* fqueue);
extern void  fileQueueDestroy(FileQueue* fqueue);

typedef struct FileStackNode {
    char* file_name;
    struct FileStackNode* next;
}FileStackNode;

// a stack struct to storage files;
typedef struct {
    FileStackNode* top;
}FileStack;

extern void  fileStackInit   (FileStack* fstk);
extern void  fileStackPush   (FileStack* fstk, char* file_name);
extern bool  fileStackIsEmpty(FileStack* fstk);
extern char* fileStackTop    (FileStack* fstk);
extern void  fileStackPop    (FileStack* fstk);
extern void  fileStackDestroy(FileStack* fstk);

#define NODE_COLOR_RED   0x00
#define NODE_COLOR_BLACK 0x01

#define NODE_CMP_LT      0x00
#define NODE_CMP_EQ      0x01
#define NODE_CMP_GT      0x02

typedef struct FileTreeNode {
    char* file_name;
    int8  color;
    struct FileTreeNode* parent;
    struct FileTreeNode* lchild;
    struct FileTreeNode* rchild;
}FileTreeNode;

// a tree struct to storage files.
typedef struct {
    FileTreeNode* root;
}FileTree;

extern void  fileTreeInit   (FileTree* ftree);
extern error fileTreeAdd    (FileTree* ftree, char* file_name);
extern bool  fileTreeExist  (FileTree* ftree, char* file_name);
extern void  fileTreeDestroy(FileTree* ftree);

#endif
