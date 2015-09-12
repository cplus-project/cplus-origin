/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "delayresolve.h"

/****** methods of DRIdentList ******/

error drIdentListInit(DRIdentList* drid_list, IdentTable* id_table) {
    if (id_table == NULL) {
        return new_error("the identifier table can not be NULL.");
    }
    drid_list->id_table = id_table;
    drid_list->head     = NULL;
    drid_list->tail     = NULL;
    return NULL;
}

bool drIdentListIsEmpty(DRIdentList* drid_list) {
    return drid_list->head == NULL ? true : false;
}

void drIdentListAdd(DRIdentList* drid_list, char* drid_name) {
    DRIdentListNode* create = (DRIdentListNode*)mem_alloc(sizeof(DRIdentListNode));
    create->drid_name = drid_name;
    create->next      = NULL;
    drid_list->head  != NULL ? (drid_list->tail->next = create) : (drid_list->head = create);
    drid_list->tail   = create;
}

error drIdentListResolve(DRIdentList* drid_list) {
    // TODO: resolve every entry in the delay resolve identifiers table...
    return NULL;
}

void drIdentListDestroy(DRIdentList* drid_list) {
    DRIdentListNode* del = NULL;
    for (;;) {
        if (drid_list->head == NULL) {
            drid_list->tail =  NULL;
            return;
        }
        del = drid_list->head;
        drid_list->head = drid_list->head->next;
        mem_free(del);
    }
}

/****** methods of DRModuleDependList ******/

void drModuleDependListInit(DRModuleDependList* list) {
    list->head = NULL;
    list->tail = NULL;
}

bool drModuleDependListIsEmpty(DRModuleDependList* list) {
    return list->head == NULL ? true : false;
}

void drModuleDependListAdd(DRModuleDependList* list, char* mod_name) {
    DRModuleDependListNode* create = (DRModuleDependListNode*)mem_alloc(sizeof(DRModuleDependListNode));
    create->mod_name = mod_name;
    create->next     = NULL;
    list->head != NULL ? (list->tail->next = create) : (list->head = create);
    list->tail  = create;
}

error drModuleDependListDel(DRModuleDependList* list, char* mod_name) {
    DRModuleDependListNode* pre = NULL;
    DRModuleDependListNode* ptr;
    for (ptr = list->head; ptr != NULL; ptr = ptr->next) {
        if (strcmp(ptr->mod_name, mod_name) == 0) {
            pre != NULL ? (pre->next = ptr->next) : (list->head = ptr->next);
            mem_free(ptr);
            return NULL;
        }
        pre = ptr;
    }
    return new_error("the module wanted to delete not in the Delay Resolve Module Dependences List.");
}

DRASTList* drModuleDependListGetDRASTList(DRModuleDependList* list) {
    return list->drast_list == NULL ? NULL : list->drast_list;
}

void drModuleDependListDestroy(DRModuleDependList* list) {
    DRModuleDependListNode* del = NULL;
    for (;;) {
        if (list->head == NULL) {
            list->tail  = NULL;
            return;
        }
        del = list->head;
        list->head = list->head->next;
        mem_free(del);
    }
}

/****** methods of DRModuleInformList ******/

void drModuleInformListInit(DRModuleInformList* list) {
    list->head = NULL;
    list->tail = NULL;
}

void drModuleInformListAdd(DRModuleInformList* list, DRModuleDependList* drmod_depend_list) {
    DRModuleInformListNode* create = (DRModuleInformListNode*)mem_alloc(sizeof(DRModuleInformList));
    create->drmod_depend_list = drmod_depend_list;
    create->next              = NULL;
    list->head != NULL ? (list->tail->next = create) : (list->head = create);
    list->tail  = create;
}

void drModuleInformListDestroy(DRModuleInformList* list) {
    DRModuleInformListNode* del = NULL;
    for (;;) {
        if (list->head == NULL) {
            list->tail  = NULL;
            return;
        }
        del = list->head;
        list->head = list->head->next;
        mem_free(del);
    }
}

/****** methods of DRASTList ******/

void drASTListInit(DRASTList* drast_list) {
    drast_list->head = NULL;
    drast_list->tail = NULL;
}

void drASTListAdd(DRASTList* drast_list, AST* ast) {
    DRASTListNode* create = (DRASTListNode*)mem_alloc(sizeof(DRASTListNode));
    create->drast = ast;
    create->next  = NULL;
    drast_list->head != NULL ? (drast_list->tail->next = create) : (drast_list->head = create);
    drast_list->tail  = create;
}

void drASTListDestroy(DRASTList* drast_list) {
    DRASTListNode* del = NULL;
    for (;;) {
        if (drast_list->head == NULL) {
            drast_list->tail  = NULL;
            return;
        }
        del = drast_list->head;
        drast_list->head = drast_list->head->next;
        astDestroy(del->drast);
        mem_free(del);
    }
}
