/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The imptcache.h and imptcache.c implement the cache
 * system for importing operation(include and module). it
 * can improve the efficiency of dependency file processing.
 **/

#ifndef CPLUS_IMPTCACHE_H
#define CPLUS_IMPTCACHE_H

#include "common.h"
#include "ident.h"

// if depend_parsed is true means that the file's dependences have been
// added in the wait queue already.
typedef struct {
    char* file_name;
    bool  depend_parsed;
}WaitCompileFile;

typedef struct CompileWaitQueueNode {
    WaitCompileFile file_info;
    struct CompileWaitQueueNode* prev;
    struct CompileWaitQueueNode* next;
}CompileWaitQueueNode;

// the CompileWaitQueue is a queue to save a set of files waiting to
// be compiled. the enqueue operation is different from the ordinary
// queues, this operation will be performed like below:
//
// ('^' is the pointer which points to the node with the filename being
// parsed at present)
//
// * now the queue's state is like this:
//   node1 -- node2 -- node3
//   ^
// * then insert the node4:
//   node4 -- node1 -- node2 -- node3
//            ^
// * then insert the node5:
//   node4 -- node5 -- node1 -- node2 -- node3
//                     ^
// * then insert the node6:
//   node4 -- node5 -- node6 -- node1 -- node2 -- node3
//                              ^
// 
// every inserted node means a dependency(include file or module). every
// node in CompileWaitQueue has a prev pointer and a next pointer, the
// inserted node should be checked that is it in CompileCacheTree already,
// if it is not in the CompileCacheTree, it should be inserted into the
// '^' node's precvious position.
//
typedef struct {
    CompileWaitQueueNode* head;
    CompileWaitQueueNode* cur;
}CompileWaitQueue;

extern void             compileWaitQueueInit   (CompileWaitQueue* waitqueue);
extern bool             compileWaitQueueIsEmpty(CompileWaitQueue* waitqueue);
extern void             compileWaitQueueEnqueue(CompileWaitQueue* waitqueue, char* file);
extern WaitCompileFile* compileWaitQueueGetFile(CompileWaitQueue* waitqueue);
extern void             compileWaitQueueDequeue(CompileWaitQueue* waitqueue);
extern void             compileWaitQueueDestroy(CompileWaitQueue* waitqueue);

#define NODE_COLOR_RED   0x00
#define NODE_COLOR_BLACK 0x01

#define NODE_CMP_LT      0x00
#define NODE_CMP_EQ      0x01
#define NODE_CMP_GT      0x02

// if the idtable is NULL, it means the file is not compiled over because
// some dependences needed.
//
typedef struct CompileCacheTreeNode {
    char*       file_name;
    IdentTable* id_table;
    int8        color;
    struct CompileCacheTreeNode* parent;
    struct CompileCacheTreeNode* lchild;
    struct CompileCacheTreeNode* rchild;
}CompileCacheTreeNode;

// the CompileCacheTree is used to save a set of files have been already
// compiled and their export symbols.
//
typedef struct {
    CompileCacheTreeNode* root;
}CompileCacheTree;

extern void        compileCacheTreeInit     (CompileCacheTree* cachetree);
extern error       compileCacheTreeCacheNew (CompileCacheTree* cachetree, char* file_name);
extern error       compileCacheTreeCacheOver(CompileCacheTree* cachetree, char* file_name, IdentTable* id_table);
extern IdentTable* compileCacheTreeCacheGet (CompileCacheTree* cachetree, char* file_name);
extern void        compileCacheTreeDestroy  (CompileCacheTree* cachetree);

#endif
