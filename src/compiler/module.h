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

extern error moduleInitByName    (Module* mod, char* mod_name, int mod_name_len, ProjectConfig* projconf);
extern error moduleInitByPath    (Module* mod, char* mod_path, int mod_path_len, ProjectConfig* projconf);
extern char* moduleGetNextSrcFile(Module* mod);
extern void  moduleRewind        (Module* mod);
extern void  moduleDisplayDetails(Module* mod);
extern void  moduleDestroy       (Module* mod);

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

extern void    moduleScheduleQueueInit      (ModuleScheduleQueue* queue);
extern bool    moduleScheduleQueueIsEmpty   (ModuleScheduleQueue* queue);
extern void    moduleScheduleQueueAddMod    (ModuleScheduleQueue* queue, Module* mod);
extern Module* moduleScheduleQueueGetHeadMod(ModuleScheduleQueue* queue);
extern void    moduleScheduleQueueDelHeadMod(ModuleScheduleQueue* queue);
extern void    moduleScheduleQueueDestroy   (ModuleScheduleQueue* queue);

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

extern void        moduleInfoDatabaseInit   (ModuleInfoDatabase* infodb);
extern bool        moduleInfoDatabaseExist  (ModuleInfoDatabase* infodb, char*       mod_name);
extern error       moduleInfoDatabaseAdd    (ModuleInfoDatabase* infodb, ModuleInfo* mod_info);
extern ModuleInfo* moduleInfoDatabaseGet    (ModuleInfoDatabase* infodb, char*       mod_name);
extern void        moduleInfoDatabaseDestroy(ModuleInfoDatabase* infodb);

//////////////////////////////////////
/*
struct ModuleScheduler {
    Module*      cur_mod;
    ModuleSet    mod_set;
    ModuleCache  mod_cache;
};

extern error moduleSchedulerInit           (ModuleScheduler* scheduler);
extern bool  moduleSchedulerIsFinish       (ModuleScheduler* scheduler);
extern char* moduleSchedulerGetPreparedFile(ModuleScheduler* scheduler);
extern void  moduleSchedulerDestroy        (ModuleScheduler* scheduler);*/

#endif
