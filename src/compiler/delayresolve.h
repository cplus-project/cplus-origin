/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The delayresolve.h and delayresolve.c implement some
 * data structs used to resolve some promblems when there are
 * some unknown identifiers or cycle module import happen.
 **/

#ifndef CPLUS_DELAYRESOLVE_H
#define CPLUS_DELAYRESOLVE_H

#include "common.h"
#include "ident.h"
#include "ast.h"

// tip: the DR means "delay resolve".
//
typedef struct DRIdentListNode        DRIdentListNode;
typedef struct DRIdentList            DRIdentList;
typedef struct DRModuleDependListNode DRModuleDependListNode;
typedef struct DRModuleDependList     DRModuleDependList;
typedef struct DRModuleInformListNode DRModuleInformListNode;
typedef struct DRModuleInformList     DRModuleInformList;
typedef struct DRASTListNode          DRASTListNode;
typedef struct DRASTList              DRASTList;
typedef struct DelayResolve           DelayResolve;

struct DRIdentListNode {
    char*            drid_name;
    DRIdentListNode* next;
};

// DRIdentList(Delay Resolve Identifiers List) is used to save the
// identifiers whose declaration is not founded yet but may be founded
// later.
//
// like this situation:
//    ...
//    say("hello") // used firstly
//    ...
//    func say(String content) { // declare or define later
//        println(content)
//    }
//    ...
//
struct DRIdentList {
    IdentTable*      id_table;
    DRIdentListNode* head;
    DRIdentListNode* tail;
};

extern error drIdentListInit   (DRIdentList* drid_list, IdentTable* id_table);
extern bool  drIdentListIsEmpty(DRIdentList* drid_list);
extern void  drIdentListAdd    (DRIdentList* drid_list, char* drid_name);
extern error drIdentListResolve(DRIdentList* drid_list);
extern void  drIdentListDestroy(DRIdentList* drid_list);

struct DRModuleDependListNode {
    char*                   mod_name;
    DRModuleDependListNode* next;
};

// DRModuleDependList(Delay Resolve Module Dependences List) is used to log the modules
// needed by now-compiling module when the cycle-import happen.
//
struct DRModuleDependList {
    DRASTList*              drast_list;
    DRModuleDependListNode* head;
    DRModuleDependListNode* tail;
};

extern void       drModuleDependListInit        (DRModuleDependList* list);
extern bool       drModuleDependListIsEmpty     (DRModuleDependList* list);
extern void       drModuleDependListAdd         (DRModuleDependList* list, char* mod_name);
extern error      drModuleDependListDel         (DRModuleDependList* list, char* mod_name);
extern DRASTList* drModuleDependListGetDRASTList(DRModuleDependList* list);
extern void       drModuleDependListDestroy     (DRModuleDependList* list);

struct DRModuleInformListNode {
    DRModuleDependList*     drmod_depend_list;
    DRModuleInformListNode* next;
};

// DRModInformList(Delay Resolve Module Infrom List) is used to inform all modules
// which need the now-compiling module what they need have been finished.
//
struct DRModuleInformList {
    DRModuleInformListNode* head;
    DRModuleInformListNode* tail;
};

extern void drModuleInformListInit   (DRModuleInformList* list);
extern void drModuleInformListAdd    (DRModuleInformList* list, DRModuleDependList* drmod_depend_list);
extern void drModuleInformListDestroy(DRModuleInformList* list);

struct DRASTListNode {
    AST*           drast;
    DRASTListNode* next;
};

// DRASTList(Delay Resolve Abstract Syntax Tree List) is used to save the ASTs which can
// not be continue to be compiled caused by cycle module import.
//
struct DRASTList {
    DRASTListNode* head;
    DRASTListNode* tail;
};

extern void drASTListInit   (DRASTList* drast_list);
extern void drASTListAdd    (DRASTList* drast_list, AST* ast);
extern void drASTListDestroy(DRASTList* drast_list);

// DelayResolve struct packs up all data structs about solving delay resolve problem.
//
struct DelayResolve {
    DRIdentList*        drid_list;
    DRModuleDependList* drmod_depd_list;
    DRModuleInformList* drmod_infm_list;
    DRASTList*          drast_list;
};

#endif
