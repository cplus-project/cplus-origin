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

#include "common.h"
#include "ident.h"
#include "lexer.h"

typedef struct SourceFile          SourceFile;
typedef struct Module              Module;
typedef struct ModuleSetNode       ModuleSetNode;
typedef struct ModuleSet           ModuleSet;
typedef struct ModuleCacheNode     ModuleCacheNode;
typedef struct ModuleCache         ModuleCache;
typedef struct DRModDependListNode DRModDependListNode;
typedef struct DRModDependList     DRModDependList;
typedef struct DRModInformListNode DRModInformListNode;
typedef struct DRModInformList     DRModInformList;
typedef struct ModuleScheduler     ModuleScheduler;

// represent a source file in one module or program directory.
struct SourceFile {
    char*       file_name;
    SourceFile* next;
};

struct Module {
    char*       mod_name; // the path of the module. "net/http" and "test/http" are two different modules
    SourceFile* srcfiles; // cplus source files in the module directory.
};

struct ModuleSetNode {
    Module* mod;
    // RecompileList
    // InternalUnresolvedList
    // ExternalUnresolvedList // extern unresolved list is not empty means that this module should be resolved
                              // later again when dependent modules are compiled over.
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

extern void  drModDependListInit   (DRModDependList* list);
extern bool  drModDependListIsEmpty(DRModDependList* list);
extern void  drModDependListAdd    (DRModDependList* list, char* mod_name);
extern error drModDependListDel    (DRModDependList* list, char* mod_name);
extern void  drModDependListDestroy(DRModDependList* list);

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

extern void drModInformListInit   (DRModInformList* list);
extern void drModInformListAdd    (DRModInformList* list, char* mod_name);
extern void drModInformListDestroy(DRModInformList* list);

// ModuleScheduler manages and caches all modules used by a project.
//
struct ModuleScheduler {
    ModuleSet   mod_set;
    ModuleCache mod_cache;
};

extern void moduleSchedulerInit   (ModuleScheduler* modsche);
extern void moduleSchedulerDestroy(ModuleScheduler* modsche);

#endif
