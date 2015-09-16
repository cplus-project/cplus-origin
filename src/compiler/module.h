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
#include "delayresolve.h"

typedef struct SourceFile              SourceFile;
typedef struct Module                  Module;
typedef struct ModuleScheduleQueueNode ModuleScheduleQueueNode;
typedef struct ModuleScheduleQueue     ModuleScheduleQueue;
typedef struct ModuleInfo              ModuleInfo;
typedef struct ModuleInfoDatabaseNode  ModuleInfoDatabaseNode;
typedef struct ModuleInfoDatabase      ModuleInfoDatabase;
typedef struct ModuleScheduler         ModuleScheduler;

// represent a source file in one module or program directory.
//
struct SourceFile {
    char*       file_name;
    int         file_name_len;
    SourceFile* next;
};

struct Module {
    ModuleInfo* mod_info;
    char*       mod_path;
    int         mod_path_len;
    bool        is_main;
    bool        dependences_parsed;
    SourceFile* srcfiles;
    SourceFile* iterator;
};

// note： the Module's memory will be allocated and released automatically by ModuleScheduler. so
//        you will never have necessity to call moduleInitXXXX and moduleDestroy.
//
static void  moduleInitByName    (Module* mod, char* mod_name, int mod_name_len, ProjectConfig* projconf);
static void  moduleInitByPath    (Module* mod, char* mod_path, int mod_path_len, ProjectConfig* projconf);
extern char* moduleGetNextSrcFile(Module* mod);
extern void  moduleRewind        (Module* mod);
static void  moduleDestroy       (Module* mod);

struct ModuleScheduleQueueNode {
    Module*                  mod;
    ModuleScheduleQueueNode* prev;
    ModuleScheduleQueueNode* next;
};

// the ModuleScheduleQueue is a liner list to save a set of modules waiting to
// be compiled. the AddMod operation of the queue will be performed like below:
//
// ('^' is the pointer which points to the module being processed now)
//
// 1. now the queue's state is like this:
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
// the queue.
//
struct ModuleScheduleQueue {
    ModuleScheduleQueueNode* head;
    ModuleScheduleQueueNode* cur;
};

static void    moduleScheduleQueueInit      (ModuleScheduleQueue* queue);
static bool    moduleScheduleQueueIsEmpty   (ModuleScheduleQueue* queue);
static void    moduleScheduleQueueAddMod    (ModuleScheduleQueue* queue, Module* mod);
static Module* moduleScheduleQueueGetHeadMod(ModuleScheduleQueue* queue);
static void    moduleScheduleQueueDelHeadMod(ModuleScheduleQueue* queue);
static void    moduleScheduleQueueDestroy   (ModuleScheduleQueue* queue);

#define NODE_COLOR_RED   0x00
#define NODE_COLOR_BLACK 0x01

#define NODE_CMP_LT      0x00
#define NODE_CMP_EQ      0x01
#define NODE_CMP_GT      0x02

struct ModuleInfo {
    char*       mod_name;
    bool        compile_over;
    IdentTable* id_table;
    // TODO: delay resolve related ...
};

struct ModuleInfoDatabaseNode {
    ModuleInfo*             mod_info;
    int8                    color;
    ModuleInfoDatabaseNode* parent;
    ModuleInfoDatabaseNode* lchild;
    ModuleInfoDatabaseNode* rchild;
};

// the ModuleInfoDatabase is used to save some information and the states of all modules
// in a cplus project.
//
struct ModuleInfoDatabase {
    ModuleInfoDatabaseNode* root;
};

static void        moduleInfoDatabaseInit   (ModuleInfoDatabase* infodb);
static bool        moduleInfoDatabaseExist  (ModuleInfoDatabase* infodb, char*       mod_name);
static error       moduleInfoDatabaseAdd    (ModuleInfoDatabase* infodb, ModuleInfo* mod_info);
static ModuleInfo* moduleInfoDatabaseGet    (ModuleInfoDatabase* infodb, char*       mod_name);
static void        moduleInfoDatabaseDestroy(ModuleInfoDatabase* infodb);

// ModuleScheduler can manage all modules needed by a cplus project. it will analyze these
// modules' relation and parse their dependences. it will always return a module which
// need fewest other modules of the current state.
//
// example:
//    TODO: make the doc here...
//
struct ModuleScheduler {
    ProjectConfig*      projconf;
    Module*             mod_cur;
    ModuleScheduleQueue mod_schd_queue;
    ModuleInfoDatabase  mod_info_db;
};

extern error   moduleSchedulerInit             (ModuleScheduler* modschdr, ProjectConfig* const projconf);
extern bool    moduleSchedulerIsFinish         (ModuleScheduler* modschdr);
extern Module* moduleSchedulerGetPreparedModule(ModuleScheduler* modschdr);
extern void    moduleSchedulerDestroy          (ModuleScheduler* modschdr);

#endif
