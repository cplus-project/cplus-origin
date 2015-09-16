/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "module.h"

/****** methods of Module ******/

// example:
//    if the source path is "/home/user/project/src".
//    the module name "net/http" will return "/home/user/project/src/net.mod/http.mod".
//                                                                   ^^^^^^^^^^^^^^^^
static char* moduleGetModPathByName(const char* const mod_name, int mod_name_len, ProjectConfig* projconf) {
    char* mod_path;
    int   i;
    DynamicArrChar darr;
    dynamicArrCharInit   (&darr, 255);
    dynamicArrCharAppend (&darr, projconf->path_source, projconf->srcdir_path_len);
    dynamicArrCharAppendc(&darr, '/');
    for (i = 0; i < mod_name_len; i++) {
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
static char* moduleGetModNameByPath(const char* const mod_path, int mod_path_len, ProjectConfig* projconf) {
    char* mod_name;
    int   i;
    DynamicArrChar darr;
    dynamicArrCharInit(&darr, 255);
    for (i = projconf->srcdir_path_len+1; i < mod_path_len;) {
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

// check whether a file is the cplus source file(which is suffixed with '.cplus'). this
// function is more efficient than using the is_cplus_source(defined in project.h and
// project.c) here.
//
static bool moduleIsSrcFile(char* file_name, int len) {
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

static SourceFile* moduleGetSrcFileList(char* dir_path, int path_len) {
    SourceFile*    head = NULL;
    SourceFile*    tail = NULL;
    SourceFile*    create;
    DIR*           dir;
    struct dirent* dir_entry;
    int            len;

    if ((dir = opendir(dir_path)) == NULL) {
        return NULL;
    }
    while ((dir_entry = readdir(dir)) != NULL) {
        len = strlen(dir_entry->d_name);
        if (dir_entry->d_type == DT_REG && moduleIsSrcFile(dir_entry->d_name, len) == true) {
            // warning:
            //    care about the dir_entry->d_name, it may cause the wild pointer. if it happened,
            //    just copy every dir_entry->d_name to the create->file_name with a new memory.
            //
            create = (SourceFile*)mem_alloc(sizeof(SourceFile));
            create->file_name     = dir_entry->d_name;
            create->file_name_len = len;
            create->next          = NULL;

            head != NULL ? (tail->next = create) : (head = create);
            tail  = create;
        }
    }
    closedir(dir);
    return head;
}

static void moduleInitByName(Module* mod, char* mod_name, int mod_name_len, ProjectConfig* projconf) {
    mod->mod_info               = (ModuleInfo*)mem_alloc(sizeof(ModuleInfo));
    mod->mod_info->mod_name     = mod_name;
    mod->mod_info->compile_over = false;
    mod->mod_info->id_table     = NULL;
    mod->mod_path               = moduleGetModPathByName(mod_name, mod_name_len, projconf);
    mod->mod_path_len           = strlen(mod->mod_path);
    mod->is_main                = false;
    mod->dependences_parsed     = false;
    mod->srcfiles               = moduleGetSrcFileList(mod->mod_path, mod->mod_path_len);
}

static void moduleInitByPath(Module* mod, char* mod_path, int mod_path_len, ProjectConfig* projconf) {
    if (is_cplus_program(mod_path, mod_path_len) == true) {
        mod->mod_info                = (ModuleInfo*)mem_alloc(sizeof(ModuleInfo));
        mod->mod_info->mod_name      = path_last(mod_path, mod_path_len);
        mod->mod_info->compile_over  = false;
        mod->mod_info->id_table      = NULL;
        mod->mod_path                = mod_path;
        mod->mod_path_len            = mod_path_len;
        mod->is_main                 = true;
        mod->dependences_parsed      = false;
        mod->srcfiles                = moduleGetSrcFileList(mod_path, mod_path_len);
    }
    else if (is_cplus_module(mod_path, mod_path_len) == true) {
        mod->mod_info                = (ModuleInfo*)mem_alloc(sizeof(ModuleInfo));
        mod->mod_info->mod_name      = moduleGetModNameByPath(mod_path, mod_path_len, projconf);
        mod->mod_info->compile_over  = false;
        mod->mod_info->id_table      = NULL;
        mod->mod_path                = mod_path;
        mod->mod_path_len            = mod_path_len;
        mod->is_main                 = false;
        mod->dependences_parsed      = false;
        mod->srcfiles                = moduleGetSrcFileList(mod_path, mod_path_len);
    }
    else if (is_cplus_source(mod_path, mod_path_len) == true) {
        mod->mod_info                = (ModuleInfo*)mem_alloc(sizeof(ModuleInfo));
        mod->mod_info->mod_name      = path_last(mod_path, mod_path_len);
        mod->mod_info->compile_over  = false;
        mod->mod_info->id_table      = NULL;
        mod->mod_path                = path_prev(mod_path, mod_path_len);
        mod->mod_path_len            = mod_path_len;
        mod->is_main                 = true;
        mod->dependences_parsed      = false;
        mod->srcfiles                = (SourceFile*)mem_alloc(sizeof(ModuleInfo));
        mod->srcfiles->file_name     = mod->mod_info->mod_name;
        mod->srcfiles->file_name_len = strlen(mod->srcfiles->file_name);
        mod->srcfiles->next          = NULL;
    }
}

char* moduleGetNextSrcFile(Module* mod) {
    if (mod->srcfiles == NULL) {
        return NULL;
    }
    char*          file;
    DynamicArrChar darr;
    dynamicArrCharInit   (&darr, 255);
    dynamicArrCharAppend (&darr, mod->mod_path, mod->mod_path_len);
    dynamicArrCharAppendc(&darr, '/');
    dynamicArrCharAppend (&darr, mod->srcfiles->file_name, mod->srcfiles->file_name_len);
    file = dynamicArrCharGetStr(&darr);
    dynamicArrCharDestroy(&darr);

    mod->srcfiles = mod->srcfiles->next;
    return file;
}

void moduleRewind(Module* mod) {
    mod->iterator = mod->srcfiles;
}

static void moduleDestroy(Module* mod) {
    mem_free(mod->mod_info->mod_name);
    if (mod->mod_info->id_table != NULL) {
        identTableDestroy(mod->mod_info->id_table);
    }
    mem_free(mod->mod_info);
//  mem_free(mod->mod_path);
    SourceFile* del;
    for (;;) {
        if (mod->srcfiles == NULL) {
            return;
        }
        del = mod->srcfiles;
        mod->srcfiles = mod->srcfiles->next;
//      mem_free(del->file_name);
        mem_free(del);
    }
}

/****** methods of ModuleScheduleQueue ******/

static void moduleScheduleQueueInit(ModuleScheduleQueue* queue) {
    queue->cur  = NULL;
    queue->head = (ModuleScheduleQueueNode*)mem_alloc(sizeof(ModuleScheduleQueueNode));
    queue->head->mod  = NULL;
    queue->head->prev = NULL;
    queue->head->next = NULL;
}

// return true if the queue is empty.
//
static bool moduleScheduleQueueIsEmpty(ModuleScheduleQueue* queue) {
    return queue->head->next == NULL ? true : false;
}

static void moduleScheduleQueueAddMod(ModuleScheduleQueue* queue, Module* mod) {
    ModuleScheduleQueueNode* create = (ModuleScheduleQueueNode*)mem_alloc(sizeof(ModuleScheduleQueueNode));
    create->mod = mod;
    if (queue->head->next != NULL) {
        create->prev = queue->cur->prev;
        create->next = queue->cur;
        queue->cur->prev->next = create;
        queue->cur->prev = create;
    }
    else {
        create->next = NULL;
        create->prev = queue->head;
        queue->head->next = create;
    }
}

// return the module which is prepared to be compiled and set the corresponding node
// to the 'cur' position. all new dependency nodes will be inserted into the previous
// position of the 'cur' position.
//
// example:
//    now state:
//       [m1.mod m2.mod m3.mod m4.mod]
//                        ^
//                       cur
//    ater call GetHeadMod:
//       [m1.mod m2.mod m3.mod m4.mod]
//          ^
//         cur
//
static Module* moduleScheduleQueueGetHeadMod(ModuleScheduleQueue* queue) {
    queue->cur = queue->head->next;
    return queue->cur != NULL ? queue->cur->mod : NULL;
}

static void moduleScheduleQueueDelHeadMod(ModuleScheduleQueue* queue) {
    ModuleScheduleQueueNode* del = queue->head->next;
    if (del != NULL) {
        if (del->next != NULL) {
            queue->head->next = del->next;
            del->next->prev = queue->head;
            del->prev = NULL;
            del->next = NULL;
        }
        else {
            queue->head->next = NULL;
            del->prev = NULL;
        }
        mem_free(del);
    }
}

static void moduleScheduleQueueDestroy(ModuleScheduleQueue* queue) {
    ModuleScheduleQueueNode* del = NULL;
    ModuleScheduleQueueNode* ptr = queue->head;
    for (;;) {
        if (ptr == NULL) {
            queue->head = NULL;
            queue->cur  = NULL;
            return;
        }
        del = ptr;
        ptr = ptr->next;

        del->prev = NULL;
        del->next = NULL;
        mem_free(del);
    }
}

/****** methods of ModuleInfoDatabase ******/

static void moduleInfoDatabaseInit(ModuleInfoDatabase* infodb) {
    infodb->root = NULL;
}

// firstly compare every character of the two name from index 0:
// (1) if name1[index] < name2[index] -> NODE_CMP_LT.
// (2) if name1[index] > name2[index] -> NODE_CMP_GT.
//
// if all characters compared are the same, then compare their names' length:
// (1) if the name1's length is longer , return NODE_CMP_GT.
// (2) if the name1's length is shorter, return NODE_CMP_LT.
// (3) if tow names'  length are equal , return NODE_CMP_EQ.
//
static int moduleInfoDatabaseCmp(char* name1, char* name2) {
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
//
static error moduleInfoDatabaseLeftRotate(ModuleInfoDatabase* infodb, ModuleInfoDatabaseNode* node) {
    if (node == infodb->root) {
        infodb->root = node->rchild;
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
//
static error moduleInfoDatabaseRightRotate(ModuleInfoDatabase* infodb, ModuleInfoDatabaseNode* node) {
    if (node == infodb->root) {
        infodb->root = node->lchild;
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
//
static void moduleInfoDatabaseAddFixup(ModuleInfoDatabase* infodb, ModuleInfoDatabaseNode* added) {
    ModuleInfoDatabaseNode* uncle = NULL;
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
                        moduleInfoDatabaseLeftRotate(infodb, added);
                    }
                    added->parent->color         = NODE_COLOR_BLACK;
                    added->parent->parent->color = NODE_COLOR_RED;
                    moduleInfoDatabaseRightRotate(infodb, added->parent->parent);
                    break;
                }
            }
            else {
                uncle = added->parent->parent->lchild;
                if (uncle == NULL || uncle->color == NODE_COLOR_BLACK) {
                    if (added == added->parent->lchild) {
                        added =  added->parent;
                        moduleInfoDatabaseRightRotate(infodb, added);
                    }
                    added->parent->color         = NODE_COLOR_BLACK;
                    added->parent->parent->color = NODE_COLOR_RED;
                    moduleInfoDatabaseLeftRotate(infodb, added->parent->parent);
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
    infodb->root->color = NODE_COLOR_BLACK;
}

// used to verify whether a cache entry is in the cache tree no matter has it been
// compiled already.
//
// return:
//    true  -> the cache entry is already in the cache tree
//    false -> the cache entry is not in the cache tree
//
static bool moduleInfoDatabaseExist(ModuleInfoDatabase* infodb, char* mod_name) {
    if (infodb->root == NULL) {
        return false;
    }
    ModuleInfoDatabaseNode* ptr = infodb->root;
    for (;;) {
        switch (moduleInfoDatabaseCmp(mod_name, ptr->mod_info->mod_name)) {
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

static error moduleInfoDatabaseAdd(ModuleInfoDatabase* infodb, ModuleInfo* mod_info) {
    if (mod_info == NULL) {
        return new_error("the module info can not be NULL.");
    }
    if (mod_info->mod_name == NULL) {
        return new_error("the module name can not be NULL.");
    }
    ModuleInfoDatabaseNode* create = (ModuleInfoDatabaseNode*)mem_alloc(sizeof(ModuleInfoDatabaseNode));
    create->mod_info  = mod_info;
    create->color     = NODE_COLOR_RED;
    create->parent    = NULL;
    create->lchild    = NULL;
    create->rchild    = NULL;
    if (infodb->root != NULL) {
        ModuleInfoDatabaseNode* ptr = infodb->root;
        for (;;) {
            switch (moduleInfoDatabaseCmp(mod_info->mod_name, ptr->mod_info->mod_name)) {
            case NODE_CMP_LT:
                if (ptr->lchild == NULL) {
                    ptr->lchild = create;
                    create->parent = ptr;
                    moduleInfoDatabaseAddFixup(infodb, create);
                    return NULL;
                }
                ptr = ptr->lchild;
                break;

            case NODE_CMP_GT:
                if (ptr->rchild == NULL) {
                    ptr->rchild = create;
                    create->parent = ptr;
                    moduleInfoDatabaseAddFixup(infodb, create);
                    return NULL;
                }
                ptr = ptr->rchild;
                break;

            case NODE_CMP_EQ:
                return new_error("the module information is already in the database.");
            }
        }
    }
    else {
        create->color = NODE_COLOR_BLACK;
        infodb->root = create;
        return NULL;
    }
}

static ModuleInfo* moduleInfoDatabaseGet(ModuleInfoDatabase* infodb, char* mod_name) {
    if (infodb->root == NULL) {
        return NULL;
    }
    ModuleInfoDatabaseNode* ptr = infodb->root;
    for (;;) {
        switch (moduleInfoDatabaseCmp(mod_name, ptr->mod_info->mod_name)) {
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
            return ptr->mod_info;
        }
    }
}

static void moduleInfoDatabaseDestroyNode(ModuleInfoDatabaseNode* node) {
    if (node != NULL) {
        if (node->lchild != NULL) moduleInfoDatabaseDestroyNode(node->lchild);
        if (node->rchild != NULL) moduleInfoDatabaseDestroyNode(node->rchild);
        if (node->mod_info->id_table != NULL) {
            identTableDestroy(node->mod_info->id_table);
        }
        mem_free(node);
    }
}

static void moduleInfoDatabaseDestroy(ModuleInfoDatabase* infodb) {
    moduleInfoDatabaseDestroyNode(infodb->root);
}

/****** methods of ModuleScheduler ******/

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

error moduleSchedulerInit(ModuleScheduler* modschdr, ProjectConfig* const projconf) {
    if (projconf == NULL) {
        return new_error("pass in the NULL project config.");
    }

    modschdr->projconf = projconf;
    modschdr->mod_cur  = NULL;
    moduleScheduleQueueInit(&modschdr->mod_schd_queue);
    moduleInfoDatabaseInit (&modschdr->mod_info_db);

    // in the switch the compile object will be added into the ModuleScheduleQueue. the compile
    // object may be a cplus source file, some program directories or a module. because the
    // process for the source file and the program directories is the some with the modules, so
    // they will be treated as a module.
    //
    error err = NULL;
    switch (projconf->compile_obj_type) {
    case COMPILE_OBJ_TYPE_PROJ: {
            Module*        mod;
            DIR*           dir;
            struct dirent* dir_entry;
            int            len;
            DynamicArrChar darr;
            dynamicArrCharInit(&darr, 255);
            // trival the 'project/src' directory and add all xxx.prog directories into the
            // module schedule queue.
            //
            if ((dir = opendir(projconf->path_source)) == NULL) {
                return new_error("open project source directory failed.");
            }
            while ((dir_entry = readdir(dir)) != NULL) {
                len = strlen(dir_entry->d_name);
                if (dir_entry->d_type == DT_DIR && moduleSchedulerIsProgDir(dir_entry->d_name, len) == true) {
                    dynamicArrCharAppend (&darr, projconf->path_source, projconf->srcdir_path_len);
                    dynamicArrCharAppendc(&darr, '/');
                    dynamicArrCharAppend (&darr, dir_entry->d_name, len);

                    mod = (Module*)mem_alloc(sizeof(Module));
                    moduleInitByPath(mod, dynamicArrCharGetStr(&darr), darr.used, projconf);
                    moduleScheduleQueueAddMod    (&modschdr->mod_schd_queue, mod);
                    moduleScheduleQueueGetHeadMod(&modschdr->mod_schd_queue);

                    dynamicArrCharClear(&darr);
                }
            }
            closedir(dir);
            dynamicArrCharDestroy(&darr);
            return NULL;
        }

    case COMPILE_OBJ_TYPE_PROG: {
            Module* mod = (Module*)mem_alloc(sizeof(Module));
            moduleInitByPath(mod, projconf->path_compile_obj, projconf->cplobj_path_len, projconf);
            moduleScheduleQueueAddMod(&modschdr->mod_schd_queue, mod);
            return NULL;
        }

    case COMPILE_OBJ_TYPE_MOD: {
            Module* mod = (Module*)mem_alloc(sizeof(Module));
            moduleInitByPath(mod, projconf->path_compile_obj, projconf->cplobj_path_len, projconf);
            moduleInfoDatabaseAdd    (&modschdr->mod_info_db   , mod->mod_info);
            moduleScheduleQueueAddMod(&modschdr->mod_schd_queue, mod);
            return NULL;
        }

    case COMPILE_OBJ_TYPE_SRC: {
            Module* mod = (Module*)mem_alloc(sizeof(Module));
            moduleInitByPath(mod, projconf->path_compile_obj, projconf->cplobj_path_len, projconf);
            moduleScheduleQueueAddMod(&modschdr->mod_schd_queue, mod);
            return NULL;
        }
    default:
        return new_error("invalid compile object type.");
    }
}

// return true if all modules are compiled over.
//
bool moduleSchedulerIsFinish(ModuleScheduler* modschdr) {
    return moduleScheduleQueueIsEmpty(&modschdr->mod_schd_queue);
}

// this function will add all modules needed by the modschdr->mod_cur.
//
// for example:
// 1. the ModuleScheduleQueue's state is: [module]
//
//                                         file1.cplus: module fmt
//                                       /              module net/http
//                                      /
// 2. now parse the module: module.mod {-- file2.cplus: module net/http
//                                      \               module os
//                                       \
//                                         file3.cplus: module io
//
// 3. now ModuleScheduleQueue's state is: [fmt net/http os io module]
//                                         ^^^ ^^^^^^^^ ^^ ^^
//    the modules underlined with '^' are added after parse the dependences.
//   
static error moduleSchedulerParseDependences(ModuleScheduler* modschdr) {
    error          err;
    char*          mod_name;
    int64          mod_name_len;
    char*          file;
    LexToken*      lextkn;
    bool           last_isid;
    Module*        mod;
    DynamicArrChar darr;
    dynamicArrCharInit(&darr, 255);

    while ((file = moduleGetNextSrcFile(modschdr->mod_cur)) != NULL) {
        Lexer     lexer;
        LexToken* lextkn;
        if ((err = lexerInit(&lexer)) != NULL) {
            return err;
        }
        if ((err = lexerOpenSrcFile(&lexer, file)) != NULL) {
            return err;
        }
        for (;;) {
            // check module keywork.
            //
            if ((err = lexerParseToken(&lexer)) != NULL) {
                break;
            }
            lextkn = lexerReadToken(&lexer);
            if (lextkn->token_code != TOKEN_KEYWORD_MODULE) {
                break;
            }
            lexerNextToken(&lexer);

            // check the module name and add the module into the ModuleScheduleQueue if it is finded
            // first time.
            //
            last_isid    = false;
            mod_name_len = 0;
            for (;;) {
                if ((err = lexerParseToken(&lexer)) != NULL) {
                    break;
                }
                lextkn = lexerReadToken(&lexer);

                if (lextkn->token_code == TOKEN_ID && last_isid == false) {
                    dynamicArrCharAppendDarr(&darr, &lextkn->token);
                    mod_name_len += lextkn->token_len;
                }
                else if (lextkn->token_code == TOKEN_OP_DIV && last_isid == true) {
                    dynamicArrCharAppendc(&darr, '/');
                    mod_name_len += 1;
                }
                else if (lextkn->token_code == TOKEN_LINEFEED && last_isid == true) {
                    mod_name = dynamicArrCharGetStr(&darr);
                    dynamicArrCharClear(&darr);

                    if (moduleInfoDatabaseExist(&modschdr->mod_info_db, mod_name) == true) {
                        // TODO: cycle import may happen...
                    }
                    else {
                        mod = (Module*)mem_alloc(sizeof(Module));
                        moduleInitByName(mod, mod_name, mod_name_len, modschdr->projconf);
                        moduleInfoDatabaseAdd    (&modschdr->mod_info_db   , mod->mod_info);
                        moduleScheduleQueueAddMod(&modschdr->mod_schd_queue, mod);
                    }
                    break;
                }
                else
                    return new_error("invalid module name.");
            }
        }
        lexerCloseSrcFile(&lexer);
        lexerDestroy(&lexer);
    }

    modschdr->mod_cur->dependences_parsed = true;
    moduleRewind(modschdr->mod_cur);
    dynamicArrCharDestroy(&darr);
    return NULL;
}

Module* moduleSchedulerGetPreparedModule(ModuleScheduler* modschdr) {
    if (modschdr->mod_cur != NULL) {
        moduleDestroy(modschdr->mod_cur);
        mem_free(modschdr->mod_cur);
    }
    while ((modschdr->mod_cur = moduleScheduleQueueGetHeadMod(&modschdr->mod_schd_queue)) != NULL) {
        if (modschdr->mod_cur->dependences_parsed == false)
            moduleSchedulerParseDependences(modschdr);
        else
            break;
    }
    return modschdr->mod_cur;
}

void moduleSchedulerDestroy(ModuleScheduler* modschdr) {
    if (modschdr->mod_cur != NULL) {
        moduleDestroy(modschdr->mod_cur);
        mem_free(modschdr->mod_cur);
    }
    moduleScheduleQueueDestroy(&modschdr->mod_schd_queue);
    moduleInfoDatabaseDestroy (&modschdr->mod_info_db);
}
