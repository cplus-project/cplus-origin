/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The test for CompileCacheTree in file imptcache.h
 * and imptcache.c
 **/

#include "../imptcache.h"

static void compileCacheTreeDebugPreOrder(CompileCacheTreeNode* node) {
    if (node != NULL) {
        printf("%s ", node->file_name);
        node->id_table != NULL ? printf("(have id table)") : printf("(not have id table)");
        printf("\r\n");
        if (node->lchild != NULL) compileCacheTreeDebugPreOrder(node->lchild);
        if (node->rchild != NULL) compileCacheTreeDebugPreOrder(node->rchild);
    }
}

static void compileCacheTreeDebugInOrder(CompileCacheTreeNode* node) {
    if (node != NULL) {
        if (node->lchild != NULL) compileCacheTreeDebugInOrder(node->lchild);
        printf("%s ", node->file_name);
        node->id_table != NULL ? printf("(have id table)") : printf("(not have id table)");
        printf("\r\n");
        if (node->rchild != NULL) compileCacheTreeDebugInOrder(node->rchild);
    }
}

static void compileCacheTreeDebugPostOrder(CompileCacheTreeNode* node) {
    if (node != NULL) {
        if (node->lchild != NULL) compileCacheTreeDebugPostOrder(node->lchild);
        if (node->rchild != NULL) compileCacheTreeDebugPostOrder(node->rchild);
        printf("%s ", node->file_name);
        node->id_table != NULL ? printf("(have id table)") : printf("(not have id table)");
        printf("\r\n");
    }
}

int main() {
    CompileCacheTree cachetree;
    compileCacheTreeInit(&cachetree);

    printf("add some nodes with the below order: f1.cplus to f9.cplus\r\n");
    compileCacheTreeCacheNew(&cachetree, "f1.cplus");
    compileCacheTreeCacheNew(&cachetree, "f2.cplus");
    compileCacheTreeCacheNew(&cachetree, "f3.cplus");
    compileCacheTreeCacheNew(&cachetree, "f4.cplus");
    compileCacheTreeCacheNew(&cachetree, "f5.cplus");
    compileCacheTreeCacheNew(&cachetree, "f6.cplus");
    compileCacheTreeCacheNew(&cachetree, "f7.cplus");
    compileCacheTreeCacheNew(&cachetree, "f8.cplus");
    compileCacheTreeCacheNew(&cachetree, "f9.cplus");
    compileCacheTreeDebugInOrder(cachetree.root);

    printf("\r\nadd the id table for nodes f1.cplus to f4.cplus:\r\n");
    compileCacheTreeCacheOver(&cachetree, "f1.cplus", (IdentTable*)mem_alloc(sizeof(IdentTable)));
    compileCacheTreeCacheOver(&cachetree, "f2.cplus", (IdentTable*)mem_alloc(sizeof(IdentTable)));
    compileCacheTreeCacheOver(&cachetree, "f3.cplus", (IdentTable*)mem_alloc(sizeof(IdentTable)));
    compileCacheTreeCacheOver(&cachetree, "f4.cplus", (IdentTable*)mem_alloc(sizeof(IdentTable)));
    compileCacheTreeDebugInOrder(cachetree.root);
    
    printf("\r\ntest the search opeartion:\r\n");
    compileCacheTreeCacheGet(&cachetree, "f1.cplus") != NULL ? printf("f1.cplus: some\r\n") : printf("f1.cplus: null\r\n");
    compileCacheTreeCacheGet(&cachetree, "f6.cplus") != NULL ? printf("f6.cplus: some\r\n") : printf("f6.cplus: null\r\n");
    compileCacheTreeCacheGet(&cachetree, "f9.cplus") != NULL ? printf("f9.cplus: some\r\n") : printf("f9.cplus: null\r\n");
    compileCacheTreeCacheGet(&cachetree, "f4.cplus") != NULL ? printf("f4.cplus: some\r\n") : printf("f4.cplus: null\r\n");
    
    compileCacheTreeDestroy(&cachetree);
    debug("\r\ntest over");
    return 0;
}
