/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "module.h"

static error err = NULL;

/****** methods of Module ******/

// example:
//    if the source path is "/home/user/project/src".
//    the module name "net/http" will return "/home/user/project/src/net.mod/http.mod".
//                                                                   ^^^^^^^^^^^^^^^^
static char* moduleGetModPathByName(const char* const mod_name, int mod_name_len, const ProjectConfig* projconf) {
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
static char* moduleGetModNameByPath(const char* const mod_path, int mod_path_len, const ProjectConfig* projconf) {
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

static void moduleInitByName(Module* mod, char* mod_name, int mod_name_len, const ProjectConfig* projconf) {
    mod->mod_name     = mod_name;
    mod->mod_path     = moduleGetModPathByName(mod_name, mod_name_len, projconf);
    mod->mod_path_len = strlen(mod->mod_path);
    mod->mod_ismain   = false;
    mod->preprocessed = false;
    mod->srcfiles     = moduleGetSrcFileList(mod->mod_path, mod->mod_path_len);
    mod->iterator     = mod->srcfiles;
}

static void moduleInitByPath(Module* mod, char* mod_path, int mod_path_len, const ProjectConfig* projconf) {
    if (is_cplus_program(mod_path, mod_path_len) == true) {
        mod->mod_name     = path_last(mod_path, mod_path_len);
        mod->mod_path     = mod_path;
        mod->mod_path_len = mod_path_len;
        mod->mod_ismain   = true;
        mod->preprocessed = false;
        mod->srcfiles     = moduleGetSrcFileList(mod_path, mod_path_len);
        mod->iterator     = mod->srcfiles;
    }
    else if (is_cplus_module(mod_path, mod_path_len) == true) {
        mod->mod_name     = moduleGetModNameByPath(mod_path, mod_path_len, projconf);
        mod->mod_path     = mod_path;
        mod->mod_path_len = mod_path_len;
        mod->mod_ismain   = false;
        mod->preprocessed = false;
        mod->srcfiles     = moduleGetSrcFileList(mod_path, mod_path_len);
        mod->iterator     = mod->srcfiles;
    }
    else if (is_cplus_source(mod_path, mod_path_len) == true) {
        mod->mod_name     = path_last(mod_path, mod_path_len);
        mod->mod_path     = path_prev(mod_path, mod_path_len);
        mod->mod_path_len = mod_path_len;
        mod->mod_ismain   = true;
        mod->preprocessed = false;
        mod->srcfiles     = (SourceFile*)mem_alloc(sizeof(SourceFile));
        mod->iterator     = mod->srcfiles;
        mod->srcfiles->file_name     = mod->mod_name;
        mod->srcfiles->file_name_len = strlen(mod->srcfiles->file_name);
        mod->srcfiles->next          = NULL;
    }
}

// return NULL if all source files are trivaled once.
//
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
//  mem_free(mod->mod_name);
//  mem_free(mod->mod_path);
    SourceFile* del;
    for (;;) {
        if (mod->srcfiles == NULL) {
            mod->iterator =  NULL;
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

/****** methods of ModuleCacheTable ******/

static void moduleCacheTableInit(ModuleCacheTable* cachetable) {
    cachetable->root = NULL;
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
static int moduleCacheTableCmp(char* name1, char* name2) {
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
static error moduleCacheTableLeftRotate(ModuleCacheTable* cachetable, ModuleCacheTableNode* node) {
    if (node == cachetable->root) {
        cachetable->root = node->rchild;
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
static error moduleCacheTableRightRotate(ModuleCacheTable* cachetable, ModuleCacheTableNode* node) {
    if (node == cachetable->root) {
        cachetable->root = node->lchild;
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
static void moduleCacheTableAddFixup(ModuleCacheTable* cachetable, ModuleCacheTableNode* added) {
    ModuleCacheTableNode* uncle = NULL;
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
                        moduleCacheTableLeftRotate (cachetable, added);
                    }
                    added->parent->color         = NODE_COLOR_BLACK;
                    added->parent->parent->color = NODE_COLOR_RED;
                    moduleCacheTableRightRotate(cachetable, added->parent->parent);
                    break;
                }
            }
            else {
                uncle = added->parent->parent->lchild;
                if (uncle == NULL || uncle->color == NODE_COLOR_BLACK) {
                    if (added == added->parent->lchild) {
                        added =  added->parent;
                        moduleCacheTableRightRotate(cachetable, added);
                    }
                    added->parent->color         = NODE_COLOR_BLACK;
                    added->parent->parent->color = NODE_COLOR_RED;
                    moduleCacheTableLeftRotate (cachetable, added->parent->parent);
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
    cachetable->root->color = NODE_COLOR_BLACK;
}

static error moduleCacheTableAdd(ModuleCacheTable* cachetable, char* mod_name, IdentTable* id_table) {
    if (mod_name == NULL) {
        return new_error("the module name can not be NULL.");
    }
    if (id_table == NULL) {
        return new_error("the identifier table can not be NULL.");
    }
    ModuleCacheTableNode* create = (ModuleCacheTableNode*)mem_alloc(sizeof(ModuleCacheTableNode));
    create->mod_name = mod_name;
    create->id_table = id_table;
    create->color    = NODE_COLOR_RED;
    create->parent   = NULL;
    create->lchild   = NULL;
    create->rchild   = NULL;
    if (cachetable->root != NULL) {
        ModuleCacheTableNode* ptr = cachetable->root;
        for (;;) {
            switch (moduleCacheTableCmp(mod_name, ptr->mod_name)) {
            case NODE_CMP_LT:
                if (ptr->lchild == NULL) {
                    ptr->lchild = create;
                    create->parent = ptr;
                    moduleCacheTableAddFixup(cachetable, create);
                    return NULL;
                }
                ptr = ptr->lchild;
                break;

            case NODE_CMP_GT:
                if (ptr->rchild == NULL) {
                    ptr->rchild = create;
                    create->parent = ptr;
                    moduleCacheTableAddFixup(cachetable, create);
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
        cachetable->root = create;
        return NULL;
    }
}

// return NULL if module does not have the cache entry.
//
static IdentTable* moduleCacheTableGet(ModuleCacheTable* cachetable, char* mod_name) {
    if (cachetable->root == NULL || mod_name == NULL) {
        return NULL;
    }
    ModuleCacheTableNode* ptr = cachetable->root;
    for (;;) {
        switch (moduleCacheTableCmp(mod_name, ptr->mod_name)) {
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

static void moduleCacheTableDestroyNode(ModuleCacheTableNode* node) {
    if (node != NULL) {
        if (node->lchild != NULL) moduleCacheTableDestroyNode(node->lchild);
        if (node->rchild != NULL) moduleCacheTableDestroyNode(node->rchild);
        if (node->id_table != NULL) {
            identTableDestroy(node->id_table);
        }
        mem_free(node->mod_name);
        mem_free(node);
    }
}

static void moduleCacheTableDestroy(ModuleCacheTable* cachetable) {
    moduleCacheTableDestroyNode(cachetable->root);
}
