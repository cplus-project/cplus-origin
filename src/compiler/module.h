/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The module.h and module.c provide the supports about
 * modular programming and modular management.
 **/

#ifndef CPLUS_IMPORT_H
#define CPLUS_IMPORT_H

#include <unistd.h>
#include <dirent.h>
#include "common.h"
#include "project.h"
#include "lexer.h"
#include "ident.h"

typedef struct SourceFile            SourceFile;
typedef struct Module                Module;
typedef struct ModuleQueueNode       ModuleQueueNode;
typedef struct ModuleQueue           ModuleQueue;
typedef struct ModuleExportTableNode ModuleExportTableNode;
typedef struct ModuleExportTable     ModuleExportTable;

// represent a source file in one module or program directory.
//
struct SourceFile {
    char*       file_name;
    int         file_name_len;
    SourceFile* next;
};

struct Module {
    char*       mod_name;
    char*       mod_path;
    int         mod_path_len;
    bool        mod_ismain;
    SourceFile* srcfiles;
    SourceFile* iterator;
};

static void  moduleInitByName    (Module* mod, char* mod_name, int mod_name_len, const ProjectConfig* projconf);
static void  moduleInitByPath    (Module* mod, char* mod_path, int mod_path_len, const ProjectConfig* projconf);
extern char* moduleGetNextSrcFile(Module* mod);
extern void  moduleRewind        (Module* mod);
static void  moduleDestroy       (Module* mod);

struct ModuleQueueNode {
    Module*          mod;
    ModuleQueueNode* next;
};

struct ModuleQueue {
    ModuleQueueNode* head;
    ModuleQueueNode* tail;
};

extern void moduleQueueInit   (ModuleQueue* modqueue);
extern void moduleQueueEnqueue(ModuleQueue* modqueue, Module* modqueue);
extern void moduleQueueDestroy(ModuleQueue* modqueue);

#define NODE_COLOR_RED   0x00
#define NODE_COLOR_BLACK 0x01

#define NODE_CMP_LT      0x00
#define NODE_CMP_EQ      0x01
#define NODE_CMP_GT      0x02

struct ModuleCacheTableNode {
    char*                 mod_name;
    IdentTable*           id_table;
    int8                  color;
    ModuleCacheTableNode* parent;
    ModuleCacheTableNode* lchild;
    ModuleCacheTableNode* rchild;
};

// the ModuleCache is used to save some information and the states of all modules
// in a cplus project.
//
struct ModuleCacheTable {
    ModuleCacheTableNode* root;
};

static void        moduleCacheTableInit   (ModuleCacheTable* cachetable);
static error       moduleCacheTableAdd    (ModuleCacheTable* cachetable, char* mod_name, IdentTable* id_table);
static IdentTable* moduleCacheTableGet    (ModuleCacheTable* cachetable, char* mod_name);
static void        moduleCacheTableDestroy(ModuleCacheTable* cachetable);

#endif
