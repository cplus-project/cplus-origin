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
    create->mod_name   = mod_name;
    create->id_table   = NULL;
    create->drid_table = NULL;
    create->color      = NODE_COLOR_RED;
    create->parent     = NULL;
    create->lchild     = NULL;
    create->rchild     = NULL;
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

static void drModDependListInit(DRModDependList* list) {
    list->head = NULL;
    list->tail = NULL;
}

static bool drModDependListIsEmpty(DRModDependList* list) {
    return list->head == NULL ? true : false;
}

static void drModDependListAdd(DRModDependList* list, char* mod_name) {
    DRModDependListNode* create = (DRModDependListNode*)mem_alloc(sizeof(DRModDependListNode));
    create->mod_name = mod_name;
    create->next     = NULL;
    list->head != NULL ? (list->tail->next = create) : (list->head = create);
    list->tail  = create;
}

static error drModDependListDel(DRModDependList* list, char* mod_name) {
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

static void drModDependListDestroy(DRModDependList* list) {
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

static void drModInformListInit(DRModInformList* list) {
    list->head = NULL;
    list->tail = NULL;
}

static void drModInformListAdd(DRModInformList* list, char* mod_name) {
    DRModInformListNode* create = (DRModInformListNode*)mem_alloc(sizeof(DRModInformList));
    create->mod_name = mod_name;
    create->next     = NULL;
    list->head != NULL ? (list->tail->next = create) : (list->head = create);
    list->tail  = create;
}

static void drModInformListDestroy(DRModInformList* list) {
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

/****** methods of ModuleScheduler ******/

// save the length of ProjectConfig.path_source because its often uses.
//
static int src_path_len;

// example:
//    if the source path is "/home/user/project/src".
//    the module name "net/http" will return "/home/user/project/src/net.mod/http.mod".
//                                                                   ^^^^^^^^^^^^^^^^
//
static char* moduleSchedulerGetModPathByName(const char* const mod_name) {
    char* mod_path;
    int   i;
    int   len = strlen(mod_name);
    DynamicArrChar darr;
    dynamicArrCharInit   (&darr, 255);
    dynamicArrCharAppend (&darr, ProjectConfig.path_source, src_path_len);
    dynamicArrCharAppendc(&darr, '/');
    for (i = 0; i < len; i++) {
        if (mod_name[i] != '/') {
            dynamicArrCharAppendc(&darr, mod_name[i]);
        }
        else {
            dynamicArrCharAppend (&darr, ".mod", 4);
            dynamicArrCharAppendc(&darr, '/');
        }
    }
    dynamicArrCharAppend (&darr, ".mod", 4);
    mod_path = dynamicArrCharGetStr(&darr);
    dynamicArrCharDestroy(&darr);
    return mod_path;
}

// example:
//    if the source path is "/home/user/project/src".
//    the path "/home/user/project/src/net.mod/http.mod" will return "net/http".
//                                     ^^^^^^^^^^^^^^^^
//
static char* moduleSchedulerGetModNameByPath(const char* const mod_path) {
    char* mod_name;
    int   i;
    int   len = strlen(mod_path);
    DynamicArrChar darr;
    dynamicArrCharInit(&darr, 255);
    for (i = src_path_len+1; i < len;) {
        if (mod_path[i]   == '.' &&
            mod_path[i+1] == 'm' &&
            mod_path[i+2] == 'o' &&
            mod_path[i+3] == 'd' ){
            i += 4;
        } else {
            dynamicArrCharAppendc(&darr, mod_path[i]);
            i++;
        }
    }
    mod_name = dynamicArrCharGetStr(&darr);
    dynamicArrCharDestroy(&darr);
    return mod_name;
}

// check whether a directory is the program directory(which is suffixed with '.prog'). this
// function is more efficient than using the is_cplus_program(defined in project.h and
// project.c) here.
//
static bool moduleSchedulerIsProgDir(char* dir_name, int len) {
    if (len > 5 &&
        dir_name[len-5] == '.' &&
        dir_name[len-4] == 'p' &&
        dir_name[len-3] == 'r' &&
        dir_name[len-2] == 'o' &&
        dir_name[len-1] == 'g' ){
        return true;
    }
    return false;
}

// check whether a file is the cplus source file(which is suffixed with '.cplus'). this
// function is more efficient than using the is_cplus_source(defined in project.h and
// project.c) here.
//
static bool moduleSchedulerIsSrcFile(char* file_name, int len) {
    if (len > 6 &&
        file_name[len-6] == '.' &&
        file_name[len-5] == 'c' &&
        file_name[len-4] == 'p' &&
        file_name[len-3] == 'l' &&
        file_name[len-2] == 'u' &&
        file_name[len-1] == 's' ){
        return true;
    }
    return false;
}

static SourceFiles* moduleSchedulerGetSrcFileList(char* dir_path, int path_len) {
    SourceFiles*   head = NULL;
    SourceFiles*   tail = NULL;
    SourceFiles*   create;
    DIR*           dir;
    struct dirent* dir_entry;
    int            len;

    if ((dir = opendir(dir_path)) == NULL) {
        return NULL;
    }
    while ((dir_entry = readdir(dir)) != NULL) {
        len = strlen(dir_entry->d_name);
        if (dir_entry->d_type == DT_REG && moduleSchedulerIsSrcFile(dir_entry->d_name, len) == true) {
            create = (SourceFiles*)mem_alloc(sizeof(SourceFiles));
            create->file_name = dir_entry->d_name;
            create->name_len  = len;
            create->next      = NULL;

            head != NULL ? (tail->next = create) : (head = create);
            tail  = create;
        }
    }
    return head;
}

// TODO: just used for debug, should delete later...
static void showModule(Module* mod) {
    printf("mod_name   : %s\r\n", mod->mod_name);
    printf("mod_path   : %s\r\n", mod->mod_path);
    printf("depd_parsed: ");
    mod->depd_parsed == false ? printf("false\r\n") : printf("true\r\n");
    printf("is_main    : ");
    mod->is_main == true ? printf("true\r\n") : printf("false\r\n");
    printf("files      : ");
    SourceFiles* ptr;
    for (ptr = mod->srcfiles; ptr != NULL; ptr = ptr->next) {
        printf("%s ", ptr->file_name);
    }
    printf("\r\n\r\n");
}

error moduleSchedulerInit(ModuleScheduler* scheduler) {
    scheduler->cur_mod = NULL;
    moduleSetInit  (&scheduler->mod_set);
    moduleCacheInit(&scheduler->mod_cache);
    src_path_len = strlen(ProjectConfig.path_source);

    // in the switch the compile object will be added into the ModSet. the compile object
    // may be a cplus source file, some program directories or a module. because the process
    // for the source file and the program directories is the some with the modules, so they
    // will be treated as a module.
    //
    switch (ProjectConfig.compile_obj_type) {
    case COMPILE_OBJ_TYPE_PROJ: {
            Module*        mod;
            DIR*           dir;
            struct dirent* dir_entry;
            int            len;
            DynamicArrChar darr;
            dynamicArrCharInit(&darr, 255);
            // trival the 'project/src' directory and add all xxx.prog directories into the
            // module set.
            //
            if ((dir = opendir(ProjectConfig.path_source)) == NULL) {
                return new_error("open project source directory failed.");
            }
            while ((dir_entry = readdir(dir)) != NULL) {
                len = strlen(dir_entry->d_name);
                if (dir_entry->d_type == DT_DIR && moduleSchedulerIsProgDir(dir_entry->d_name, len) == true) {
                    dynamicArrCharAppend (&darr, ProjectConfig.path_source, src_path_len);
                    dynamicArrCharAppendc(&darr, '/');
                    dynamicArrCharAppend (&darr, dir_entry->d_name, len);

                    mod = (Module*)mem_alloc(sizeof(Module));
                    mod->mod_name    = dir_entry->d_name;
                    mod->mod_path    = dynamicArrCharGetStr(&darr);
                    mod->path_len    = darr.used;
                    mod->depd_parsed = false;
                    mod->is_main     = true;
                    mod->srcfiles    = moduleSchedulerGetSrcFileList(dynamicArrCharGetStr(&darr), src_path_len+len+1);

                    moduleSetAdd(&scheduler->mod_set, mod);
                    moduleSetGet(&scheduler->mod_set);
                    dynamicArrCharClear(&darr);
                }
            }
            closedir(dir);
            dynamicArrCharDestroy(&darr);
            return NULL;
        }

    case COMPILE_OBJ_TYPE_PROG: {
            int     len = strlen(ProjectConfig.path_compile_obj);
            Module* mod = (Module*)mem_alloc(sizeof(Module));
            mod->mod_name    = path_last(ProjectConfig.path_compile_obj, len);
            mod->mod_path    = ProjectConfig.path_compile_obj;
            mod->path_len    = len;
            mod->depd_parsed = false;
            mod->is_main     = true;
            mod->srcfiles    = moduleSchedulerGetSrcFileList(ProjectConfig.path_compile_obj, len);
            moduleSetAdd(&scheduler->mod_set, mod);
            return NULL;
        }

    case COMPILE_OBJ_TYPE_MOD: {
            int     len = strlen(ProjectConfig.path_compile_obj);
            Module* mod = (Module*)mem_alloc(sizeof(Module));
            mod->mod_name    = moduleSchedulerGetModNameByPath(ProjectConfig.path_compile_obj);
            mod->mod_path    = ProjectConfig.path_compile_obj;
            mod->path_len    = len;
            mod->depd_parsed = false;
            mod->is_main     = false;
            mod->srcfiles    = moduleSchedulerGetSrcFileList(ProjectConfig.path_compile_obj, strlen(ProjectConfig.path_compile_obj));
            moduleSetAdd(&scheduler->mod_set, mod);
            return NULL;
        }

    case COMPILE_OBJ_TYPE_SRC: {
            int     len = strlen(ProjectConfig.path_compile_obj);
            Module* mod = (Module*)mem_alloc(sizeof(Module));
            mod->mod_name    = path_last(ProjectConfig.path_compile_obj, len);
            mod->mod_path    = ProjectConfig.path_source;
            mod->path_len    = strlen(mod->mod_path);
            mod->depd_parsed = false;
            mod->is_main     = true;
            mod->srcfiles    = (SourceFiles*)mem_alloc(sizeof(SourceFiles));
            mod->srcfiles->file_name = path_last(ProjectConfig.path_compile_obj, len);
            mod->srcfiles->next      = NULL;
            moduleSetAdd(&scheduler->mod_set, mod);
            return NULL;
        }

    default:
        return new_error("maybe you are not initialize the ProjectConfig.");
    }
}

// return true if all modules are compiled over.
//
bool moduleSchedulerIsFinish(ModuleScheduler* scheduler) {
    return moduleSetIsEmpty(&scheduler->mod_set);
}

static char* moduleSchedulerGetOneFileFromMod(Module* module) {
    char*          path;
    SourceFiles*   del = module->srcfiles;
    DynamicArrChar darr;
    dynamicArrCharInit   (&darr, 255);
    dynamicArrCharAppend (&darr, module->mod_path, module->path_len);
    dynamicArrCharAppendc(&darr, '/');
    dynamicArrCharAppend (&darr, module->srcfiles->file_name, module->srcfiles->name_len);
    path = dynamicArrCharGetStr(&darr);
    dynamicArrCharDestroy(&darr);
    module->srcfiles = module->srcfiles->next;
    mem_free(del);

    return path;
}

static char* moduleSchedulerParseDependences(Module* module) {
    
}

char* moduleSchedulerGetPreparedFile(ModuleScheduler* scheduler) {
    char*          file_prepared;
    DynamicArrChar darr;
    dynamicArrCharInit(&darr, 255);

    if (scheduler->cur_mod->srcfiles != NULL) {
        return moduleSchedulerGetOneFileFromMod(scheduler->cur_mod);
    }
    else {
        moduleSetDel(&scheduler->mod_set);
        for (;;) {
            if (moduleSetIsEmpty(&scheduler->mod_set) == true) {
                return NULL;
            }
            scheduler->cur_mod = moduleSetGet(&scheduler->mod_set);
            if (scheduler->cur_mod->depd_parsed == false) {
                // TODO: parse dependences...
            }
            else {
                return moduleSchedulerGetOneFileFromMod(scheduler->cur_mod);
            }
        }
    }
}

void moduleSchedulerDestroy(ModuleScheduler* scheduler) {
    moduleSetDestroy  (&scheduler->mod_set);
    moduleCacheDestroy(&scheduler->mod_cache);
}
