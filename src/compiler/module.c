/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "module.h"

/****** methods of ModuleSet ******/

static void moduleSetInit(ModuleSet* modset) {
    modset->cur  = NULL;
    modset->head = (ModuleSetNode*)mem_alloc(sizeof(ModuleSetNode));
    modset->head->mod  = NULL;
    modset->head->prev = NULL;
    modset->head->next = NULL;
    // TODO: some members still should be set...
}

// return true if the set is empty.
static bool moduleSetIsEmpty(ModuleSet* modset) {
    return modset->head->next == NULL ? true : false;
}

static void moduleSetAdd(ModuleSet* modset, Module* mod) {
    ModuleSetNode* create = (ModuleSetNode*)mem_alloc(sizeof(ModuleSetNode));
    create->mod = mod;
    if (modset->head->next != NULL) {
        create->prev = modset->cur->prev;
        create->next = modset->cur;
        modset->cur->prev->next = create;
        modset->cur->prev = create;
    }
    else {
        create->next = NULL;
        create->prev = modset->head;
        modset->head->next = create;
    }
}

// return the module which is prepared to be compiled and set the corresponding node
// to the 'cur' position. all new dependency nodes will be inserted into the previous
// position of the 'cur' position.
//
static Module* moduleSetGet(ModuleSet* modset) {
    modset->cur = modset->head->next;
    return modset->cur != NULL ? modset->cur->mod : NULL;
}

static void moduleSetDel(ModuleSet* modset) {
    ModuleSetNode* del = modset->head->next;
    if (del != NULL) {
        if (del->next != NULL) {
            modset->head->next = del->next;
            del->next->prev = modset->head;
            del->prev = NULL;
            del->next = NULL;
        }
        else {
            modset->head->next = NULL;
            del->prev = NULL;
        }
        mem_free(del);
    }
}

static void moduleSetDestroy(ModuleSet* modset) {
    ModuleSetNode* del = NULL;
    ModuleSetNode* ptr = modset->head;
    for (;;) {
        if (ptr == NULL) {
            modset->head = NULL;
            modset->cur  = NULL;
            return;
        }
        del = ptr;
        ptr = ptr->next;

        del->prev = NULL;
        del->next = NULL;
        mem_free(del->mod);
        mem_free(del);
    }
}

/****** methods of ModuleCache ******/

static void moduleCacheInit(ModuleCache* modcache) {
    modcache->root = NULL;
}

// firstly compare every character of the two name from index 0:
// (1) if name1[index] < name2[index] -> NODE_CMP_LT.
// (2) if name1[index] > name2[index] -> NODE_CMP_GT.
//
// if all characters compared are the same, then compare their names' length:
// (1) if the name1's length is longer , return NODE_CMP_GT.
// (2) if the name1's length is shorter, return NODE_CMP_LT.
// (3) if tow names'  length are equal , return NODE_CMP_EQ.
static int moduleCacheCmp(char* name1, char* name2) {
    int64 i;
    for (i = 0; ; i++) {
        if (name1[i] < name2[i]) {
            return NODE_CMP_LT;
        }
        if (name1[i] > name2[i]) {
            return NODE_CMP_GT;
        }
        if (name1[i] == '\0' && name2[i] == '\0') {
            return NODE_CMP_EQ;
        }
        if (name1[i] == '\0' && name2[i] != '\0') {
            return NODE_CMP_LT;
        }
        if (name1[i] != '\0' && name2[i] == '\0') {
            return NODE_CMP_GT;
        }
    }
}

// example:
//     |                       |
//    node                   rchild
//   /    \      ----\      /      \
//  a   rchild   ----/    node      c
//     /      \          /    \
//    b        c        a      b
static error moduleCacheLeftRotate(ModuleCache* modcache, ModuleCacheNode* node) {
    if (node == modcache->root) {
        modcache->root = node->rchild;
        node->rchild->parent = NULL;
    }
    else {
        (node == node->parent->lchild)?
        (node->parent->lchild = node->rchild):
        (node->parent->rchild = node->rchild);
         node->rchild->parent = node->parent;
    }
    node->parent = node->rchild;
    node->rchild = node->rchild->lchild;
    if (node->parent->lchild != NULL) {
        node->parent->lchild->parent = node;
    }
    node->parent->lchild = node;
}

// example:
//        |                |
//       node            lchild
//      /    \  ----\   /      \
//   lchild   c ----/  a      node
//  /      \                 /    \
// a        b               b      c
static error moduleCacheRightRotate(ModuleCache* modcache, ModuleCacheNode* node) {
    if (node == modcache->root) {
        modcache->root = node->lchild;
        node->lchild->parent = NULL;
    }
    else {
        (node == node->parent->lchild)?
        (node->parent->lchild = node->lchild):
        (node->parent->rchild = node->lchild);
         node->lchild->parent = node->parent;
    }
    node->parent = node->lchild;
    node->lchild = node->lchild->rchild;
    if (node->parent->rchild != NULL) {
        node->parent->rchild->parent = node;
    }
    node->parent->rchild = node;
}

// fix the balance of the tree and try to keep the properties
// of the red black tree.
static void moduleCacheAddFixup(ModuleCache* modcache, ModuleCacheNode* added) {
    ModuleCacheNode* uncle = NULL;
    for (;;) {
        if (added->parent == NULL || added->parent->parent == NULL)
            break;

        if (added->color == NODE_COLOR_RED && added->parent->color == NODE_COLOR_RED) {
            // the uncle is black or NULL
            if (added->parent == added->parent->parent->lchild) {
                uncle = added->parent->parent->rchild;
                if (uncle == NULL || uncle->color == NODE_COLOR_BLACK) {
                    if (added == added->parent->rchild) {
                        added =  added->parent;
                        moduleCacheLeftRotate(modcache, added);
                    }
                    added->parent->color         = NODE_COLOR_BLACK;
                    added->parent->parent->color = NODE_COLOR_RED;
                    moduleCacheRightRotate(modcache, added->parent->parent);
                    break;
                }
            }
            else {
                uncle = added->parent->parent->lchild;
                if (uncle == NULL || uncle->color == NODE_COLOR_BLACK) {
                    if (added == added->parent->lchild) {
                        added =  added->parent;
                        moduleCacheRightRotate(modcache, added);
                    }
                    added->parent->color         = NODE_COLOR_BLACK;
                    added->parent->parent->color = NODE_COLOR_RED;
                    moduleCacheLeftRotate(modcache, added->parent->parent);
                    break;
                }
            }

            // the uncle is red
            uncle->color         = NODE_COLOR_BLACK;
            uncle->parent->color = NODE_COLOR_RED;
            added->parent->color = NODE_COLOR_BLACK;
            added = added->parent->parent;
        }
        else {
            break;
        }
    }
    modcache->root->color = NODE_COLOR_BLACK;
}

// used to verify whether a cache entry is in the cache tree no matter has it been
// compiled already.
//
// return:
//    true  -> the cache entry is already in the cache tree
//    false -> the cache entry is not in the cache tree
//
static bool moduleCacheExist(ModuleCache* modcache, char* mod_name) {
    if (modcache->root == NULL) {
        return false;
    }
    ModuleCacheNode* ptr = modcache->root;
    for (;;) {
        switch (moduleCacheCmp(mod_name, ptr->mod_name)) {
        case NODE_CMP_LT:
            if (ptr->lchild != NULL) {
                ptr = ptr->lchild;
                break;
            }
            return false;

        case NODE_CMP_GT:
            if (ptr->rchild != NULL) {
                ptr = ptr->rchild;
                break;
            }
            return false;

        case NODE_CMP_EQ:
            return true;
        }
    }
}

// just create a new cache entry in the cache tree. the new cache entry's identifier
// table is set to NULL.
//
static error moduleCacheNewCache(ModuleCache* modcache, char* mod_name) {
    if (mod_name == NULL) {
        return new_error("the module name can not be NULL.");
    }
    ModuleCacheNode* create = (ModuleCacheNode*)mem_alloc(sizeof(ModuleCacheNode));
    create->mod_name = mod_name;
    create->id_table = NULL;
    create->color    = NODE_COLOR_RED;
    create->parent   = NULL;
    create->lchild   = NULL;
    create->rchild   = NULL;
    if (modcache->root != NULL) {
        ModuleCacheNode* ptr = modcache->root;
        for (;;) {
            switch (moduleCacheCmp(mod_name, ptr->mod_name)) {
            case NODE_CMP_LT:
                if (ptr->lchild == NULL) {
                    ptr->lchild = create;
                    create->parent = ptr;
                    moduleCacheAddFixup(modcache, create);
                    return NULL;
                }
                ptr = ptr->lchild;
                break;

            case NODE_CMP_GT:
                if (ptr->rchild == NULL) {
                    ptr->rchild = create;
                    create->parent = ptr;
                    moduleCacheAddFixup(modcache, create);
                    return NULL;
                }
                ptr = ptr->rchild;
                break;

            case NODE_CMP_EQ:
                return new_error("the module's cache entry has been already in the cache tree.");
            }
        }
    }
    else {
        create->color = NODE_COLOR_BLACK;
        modcache->root = create;
        return NULL;
    }
}

// when a module is compiled over and its identifier table is exported successfully,
// the identifier table should be assigned to the associate cache entry in the cache
// tree.
//
static error moduleCacheSetCache(ModuleCache* modcache, char* mod_name, IdentTable* id_table) {
    if (modcache->root == NULL) {
        return NULL;
    }
    if (id_table == NULL) {
        return new_error("can not set the identifier table to NULL.");
    }
    ModuleCacheNode* ptr = modcache->root;
    for (;;) {
        switch (moduleCacheCmp(mod_name, ptr->mod_name)) {
        case NODE_CMP_LT:
            if (ptr->lchild != NULL) {
                ptr = ptr->lchild;
                break;
            }
            return new_error("not found the module's cache entry in cache tree.");

        case NODE_CMP_GT:
            if (ptr->rchild != NULL) {
                ptr = ptr->rchild;
                break;
            }
            return new_error("not found the module's cache entry in cache tree.");

        case NODE_CMP_EQ:
            if (id_table != NULL) {
                ptr->id_table = id_table;
                return NULL;
            }
            return new_error("assign a NULL identifier table to the module cache entry.");
        }
    }
}

// get the module's id table. if the module doesn't have the entry in the cache tree
// or the module only has an entry but without compiled yet, a NULL will be returned.
//
// return:
//       NULL -> the module is not in the cache tree or the module is not compiled over
//   NOT NULL -> the module is compiled over
//
static IdentTable* moduleCacheGetCache(ModuleCache* modcache, char* mod_name) {
    if (modcache->root == NULL) {
        return NULL;
    }
    ModuleCacheNode* ptr = modcache->root;
    for (;;) {
        switch (moduleCacheCmp(mod_name, ptr->mod_name)) {
        case NODE_CMP_LT:
            if (ptr->lchild != NULL) {
                ptr = ptr->lchild;
                break;
            }
            return NULL;

        case NODE_CMP_GT:
            if (ptr->rchild != NULL) {
                ptr = ptr->rchild;
                break;
            }
            return NULL;

        case NODE_CMP_EQ:
            return ptr->id_table;
        }
    }
}

static void moduleCacheDestroyNode(ModuleCacheNode* node) {
    if (node != NULL) {
        if (node->lchild != NULL) moduleCacheDestroyNode(node->lchild);
        if (node->rchild != NULL) moduleCacheDestroyNode(node->rchild);
        if (node->id_table != NULL) {
            identTableDestroy(node->id_table);
        }
        mem_free(node);
    }
}

static void moduleCacheDestroy(ModuleCache* modcache) {
    moduleCacheDestroyNode(modcache->root);
}

/****** methods of DRModDependList ******/

void drModDependListInit(DRModDependList* list) {
    list->head = NULL;
    list->tail = NULL;
}

bool drModDependListIsEmpty(DRModDependList* list) {
    return list->head == NULL ? true : false;
}

void drModDependListAdd(DRModDependList* list, char* mod_name) {
    DRModDependListNode* create = (DRModDependListNode*)mem_alloc(sizeof(DRModDependListNode));
    create->mod_name = mod_name;
    list->head != NULL ? (list->tail->next = create) : (list->head = create);
    list->tail  = create;
}

error drModDependListDel(DRModDependList* list, char* mod_name) {
    DRModDependListNode* pre = NULL;
    DRModDependListNode* ptr;
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

void drModDependListDestroy(DRModDependList* list) {
    DRModDependListNode* del = NULL;
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

/****** methods of DRModInformList ******/

void drModInformListInit(DRModInformList* list) {
    list->head = NULL;
    list->tail = NULL;
}

void drModInformListAdd(DRModInformList* list, char* mod_name) {
    DRModInformListNode* create = (DRModInformListNode*)mem_alloc(sizeof(DRModInformList));
    create->mod_name = mod_name;
    list->head != NULL ? (list->tail->next = create) : (list->head = create);
    list->tail  = create;
}

void drModInformListDestroy(DRModInformList* list) {
    DRModInformListNode* del = NULL;
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
