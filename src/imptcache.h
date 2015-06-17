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

typedef struct CompileWaitQueueNode {
    char* file;
    bool  parsed;
    struct CompileWaitQueueNode* prev;
    struct CompileWaitQueueNode* next;
}CompileWaitQueueNode;

typedef struct {
    CompileWaitQueueNode* head;
}CompileWaitQueue;

static void compileWaitQueueInit   (CompileWaitQueue* waitqueue);
static bool compileWaitQueueIsEmpty(CompileWaitQueue* waitqueue);
static void compileWaitQueueEnqueue(CompileWaitQueue* waitqueue, char* file);
static void compileWaitQueueDequeue(CompileWaitQueue* waitqueue);
static void compileWaitQueueDestroy(CompileWaitQueue* waitqueue);

typedef struct CompileCacheTreeNode {
    char*       file;
    IdentTable* idtable;
    int8        color;
    struct CompileCacheTreeNode* parent;
    struct CompileCacheTreeNode* lchild;
    struct CompileCacheTreeNode* rchild;
}CompileCacheTreeNode;

static void        compileCacheTreeInit   (CompileCacheTree* cachetree);
static void        compileCacheTreeAdd    (CompileCacheTree* cachetree);
static IdentTable* compileCacheTreeSearch (CompileCacheTree* cachetree);
static void        compileCacheTreeDestroy(CompileCacheTree* cachetree);

typedef struct {
    CompileCacheTreeNode* root;
}CompileCacheTree;

typedef struct {
    CompileWaitQueue waitqueue;
    CompileCacheTree cachetree;
}ImportCache;

#endif
