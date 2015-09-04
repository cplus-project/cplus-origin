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
#include "ident.h"
#include "lexer.h"

typedef struct SourceFiles         SourceFiles;
typedef struct Module              Module;
typedef struct ModuleSetNode       ModuleSetNode;
typedef struct ModuleSet           ModuleSet;
typedef struct ModuleCacheNode     ModuleCacheNode;
typedef struct ModuleCache         ModuleCache;
typedef struct Program             Program;
typedef struct ProgramQueueNode    ProgramQueueNode;
typedef struct ProgramQueue        ProgramQueue;
typedef struct DRModDependListNode DRModDependListNode;
typedef struct DRModDependList     DRModDependList;
typedef struct DRModInformListNode DRModInformListNode;
typedef struct DRModInformList     DRModInformList;
typedef struct CompileScheduler    CompileScheduler;

// represent a source file in one module or program directory.
struct SourceFiles {
    char*        file_name;
    SourceFiles* next;
};

struct Module {
    char*        mod_name; // the path of the module. "net/http" and "test/http" are two different modules
    SourceFiles* srcfiles; // cplus source files in the module directory.
};

struct ModuleSetNode {
    Module*        mod;
    ModuleSetNode* prev;
    ModuleSetNode* next;
};

// the ModuleSet is a liner list to save a set of modules waiting to
// be compiled. the add operation of the set will be performed like below:
//
// ('^' is the pointer which points to the module being processed now)
//
// 1. now the set's state is like this:
//    mod1 --- mod2 --- mod3
//    ^
// 2. then add the mod4:
//    mod4 --- mod1 --- mod2 --- mod3
//             ^
// 3. then add the mod5:
//    mod4 --- mod5 --- mod1 --- mod2 --- mod3
//                      ^
// 4. then add the mod6:
//    mod4 --- mod5 --- mod6 --- mod1 --- mod2 --- mod3
//                               ^
// every added node means a dependency required by other nodes at the right of
// the set.
//
struct ModuleSet {
    ModuleSetNode* head;
    ModuleSetNode* cur;
};

static void    moduleSetInit   (ModuleSet* modset);
static bool    moduleSetIsEmpty(ModuleSet* modset);
static void    moduleSetAdd    (ModuleSet* modset, Module* mod);
static Module* moduleSetGet    (ModuleSet* modset);
static void    moduleSetDel    (ModuleSet* modset);
static void    moduleSetDestroy(ModuleSet* modset);

#define NODE_COLOR_RED   0x00
#define NODE_COLOR_BLACK 0x01

#define NODE_CMP_LT      0x00
#define NODE_CMP_EQ      0x01
#define NODE_CMP_GT      0x02

// if the idtable is NULL, it means the file is not compiled over because
// some dependences needed.
//
struct ModuleCacheNode {
    char*            mod_name;
    IdentTable*      id_table;
    DRIdentTable*    drid_table;
    int8             color;
    ModuleCacheNode* parent;
    ModuleCacheNode* lchild;
    ModuleCacheNode* rchild;
};

// the ModuleCache is used to cache the id table of the modules which have been
// compiled.
//
struct ModuleCache {
    ModuleCacheNode* root;
};

static void        moduleCacheInit    (ModuleCache* modcache);
static bool        moduleCacheExist   (ModuleCache* modcache, char* mod_name);
static error       moduleCacheNewCache(ModuleCache* modcache, char* mod_name);
static error       moduleCacheSetCache(ModuleCache* modcache, char* mod_name, IdentTable* id_table);
static IdentTable* moduleCacheGetCache(ModuleCache* modcache, char* mod_name);
static void        moduleCacheDestroy (ModuleCache* modcache);

struct Program {
    char*         prog_name;
    SourceFiles*  srcfiles;
    IdentTable*   id_table;
    DRIdentTable* drid_table;
};

struct ProgramQueueNode {
    Program*          prog;
    ProgramQueueNode* next;
};

// the ProgramQueue is used to save the program directories of the project.
//
struct ProgramQueue {
    ProgramQueueNode* head;
    ProgramQueueNode* tail;
};

static void     programQueueInit   (ProgramQueue* queue);
static void     programQueueEnqueue(ProgramQueue* queue, Program* prog);
static Program* programQueueGet    (ProgramQueue* queue);
static void     programQueueDequeue(ProgramQueue* queue);
static void     programQueueDestroy(ProgramQueue* queue);

struct DRModDependListNode {
    char*                mod_name;
    DRModDependListNode* next;
};

// DRModDependList(Delay Resolve Module Dependences List) is used to log the modules
// needed by now-compiling module when the cycle-import happen.
// 
// tip: the structs which named prefixed with 'DR' are often used when cycle import
//      happen.
//
struct DRModDependList {
    DRModDependListNode* head;
    DRModDependListNode* tail;
};

static void  drModDependListInit   (DRModDependList* list);
static bool  drModDependListIsEmpty(DRModDependList* list);
static void  drModDependListAdd    (DRModDependList* list, char* mod_name);
static error drModDependListDel    (DRModDependList* list, char* mod_name);
static void  drModDependListDestroy(DRModDependList* list);

struct DRModInformListNode {
    char*                mod_name;
    Module*              mod_node;
    DRModInformListNode* next;
};

// DRModInformList(Delay Resolve Module Infrom List) is used to inform all modules
// which need the now-compiling module what they need have been finished.
//
// tip: the structs which named prefixed with 'DR' are often used when cycle import
//      happen.
//
struct DRModInformList {
    DRModInformListNode* head;
    DRModInformListNode* tail;
};

static void drModInformListInit   (DRModInformList* list);
static void drModInformListAdd    (DRModInformList* list, char* mod_name);
static void drModInformListDestroy(DRModInformList* list);

// warning:
//    must initialize the ProjectConfig(define in project.h and project.c) before
//    using the CompileScheduler!!!
//
struct CompileScheduler {
    Module*     mod_prepared;
    ModuleSet   mod_set;
    ModuleCache mod_cache;
};

extern error compileSchedulerInit           (CompileScheduler* scheduler);
extern bool  compileSchedulerIsFinish       (CompileScheduler* scheduler);
extern char* compileSchedulerGetPreparedFile(CompileScheduler* scheduler);
extern void  compileSchedulerDestroy        (CompileScheduler* scheduler);

#endif
