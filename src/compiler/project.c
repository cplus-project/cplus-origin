/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "project.h"

bool is_cplus_source(char* path, int path_len) {
    if (path_len < 7 || path_isreg(path) != NULL) {
        return false;
    }
    if (path[path_len-6] == '.' &&
        path[path_len-5] == 'c' &&
        path[path_len-4] == 'p' &&
        path[path_len-3] == 'l' &&
        path[path_len-2] == 'u' &&
        path[path_len-1] == 's' ){
        return true;
    }
    return false;
}

bool is_cplus_module(char* path, int path_len) {
    if (path_len < 5 || path_isdir(path) != NULL) {
        return false;
    }
    if (path[path_len-4] == '.' &&
        path[path_len-3] == 'm' &&
        path[path_len-2] == 'o' &&
        path[path_len-1] == 'd' ){
        return true;
    }
    return false;
}

bool is_cplus_program(char* path, int path_len) {
    if (path_len < 6 || path_isdir(path) != NULL) {
        return false;
    }
    if (path[path_len-5] == '.' &&
        path[path_len-4] == 'p' &&
        path[path_len-3] == 'r' &&
        path[path_len-2] == 'o' &&
        path[path_len-1] == 'g' ){
        return true;
    }
    return false;
}

bool is_cplus_project(char* path, int path_len) {
    // /project <-- check it
    //     /bin
    //     /src
    if (path_isdir(path) != NULL) {
        return false;
    }

    DynamicArrChar darr;
    dynamicArrCharInit(&darr, 255);

    // /project
    //     /bin <-- check it
    //     /src
    dynamicArrCharAppend(&darr, path, path_len);
    dynamicArrCharAppend(&darr, "/bin", 4);
    char* bin_path = dynamicArrCharGetStr(&darr);
    if (path_isdir(bin_path) != NULL) {
        mem_free(bin_path);
        dynamicArrCharDestroy(&darr);
        return false;
    }
    mem_free(bin_path);
    dynamicArrCharClear(&darr);

    // /project
    //     /bin
    //     /src <-- check it
    dynamicArrCharAppend(&darr, path, path_len);
    dynamicArrCharAppend(&darr, "/src", 4);
    char* src_path = dynamicArrCharGetStr(&darr);
    if (path_isdir(src_path) != NULL) {
        mem_free(src_path);
        dynamicArrCharDestroy(&darr);
        return false;
    }
    mem_free(src_path);
    dynamicArrCharDestroy(&darr);
    return true;
}

static void set_project_dir(char* path, int path_len) {
    ProjectConfig.path_project = path;
    DynamicArrChar darr;
    dynamicArrCharInit(&darr, 255);
    dynamicArrCharAppend(&darr, path, path_len);
    dynamicArrCharAppend(&darr, "/src", 4);
    ProjectConfig.path_source = dynamicArrCharGetStr(&darr);
    dynamicArrCharClear(&darr);
    dynamicArrCharAppend(&darr, path, path_len);
    dynamicArrCharAppend(&darr, "/bin", 4);
    ProjectConfig.path_binary = dynamicArrCharGetStr(&darr);
    dynamicArrCharDestroy(&darr);
}

static error find_and_set_project_dir(char* path, int path_len) {
    // because the compile object like .mod or .prog files must be away from
    // the project directory with at least 2 level. so we get the path's
    // parent path firstly to avoid the situation like:
    // /project
    //     /src
    //     /bin
    //     /xxx.mod (or xxx.prog)
    //
    char* path_cur = path_prev(path, path_len);
    int   len      = strlen(path_cur);
    
    char* path_del;
    for (;;) {
        path_del = path_cur;
        path_cur = path_prev(path_cur, len);
        mem_free(path_del);
        if (path_cur == NULL) {
            return new_error("invalid cplus project structure.");
        }
        len = strlen(path_cur);
        if (is_cplus_project(path_cur, len) == true) {
            set_project_dir(path_cur, len);
            return NULL;
        }
    }
}

error projectConfigInit(char* compiler_path, char* compile_obj_path) {
    if (compiler_path == NULL || compile_obj_path == NULL) {
        return new_error("the parameters can not be NULL.");
    }
    
    char* path;
    int   len = strlen(compile_obj_path);

    // the path of compile_obj_path should always be an absolute path. the example which
    // can cause some trouble will like this:
    // 
    // [directory struct]
    //    /project
    //        /bin
    //        /src
    //           /buffer.mod <- we are in here now
    //              /buffer.cplus
    //           /remote.mod
    //              /remote.cplus
    //                      
    // pwd is "~/project/src/buffer.mod"
    //
    // the compile command like "cplus build ../remote.mod" will cause some trouble when
    // the compiler want to find the project directory from the path which has "../" or
    // "./" by calling path_prev(defined in path.h and path.c).
    //
#ifdef PLATFORM_WINDOWS
    if (len >= 3 && (('a' <= compile_obj_path[0] && compile_obj_path[0] <= 'z') || ('A' <= compile_obj_path[0] && compile_obj_path[0] <= 'Z')) && compile_obj_path[1] == ':' && compile_obj_path[2] == '\\') {
        path = compile_obj_path;
    }
#else
    if (len >= 1 && compile_obj_path[0] == '/') {
        path = compile_obj_path;
    }
#endif
    else {
        if ((path = getcwd(NULL, 0)) == NULL) {
            return new_error("get current directory path failed.");
        }
        DynamicArrChar darr;
        dynamicArrCharInit   (&darr, 255);
        dynamicArrCharAppend (&darr, path, strlen(path));
        dynamicArrCharAppendc(&darr, '/');
        dynamicArrCharAppend (&darr, compile_obj_path, len);
        mem_free(path);
        path = dynamicArrCharGetStr(&darr);
        len  = darr.used;
        dynamicArrCharDestroy(&darr);
    }

    ProjectConfig.path_compiler    = compiler_path;
    ProjectConfig.path_stdmod      = NULL;
    ProjectConfig.path_project     = NULL;
    ProjectConfig.path_source      = NULL;
    ProjectConfig.path_binary      = NULL;
    ProjectConfig.path_compile_obj = path;

    if (is_cplus_project(path, len) == true) {
        ProjectConfig.compile_obj_type = COMPILE_OBJ_TYPE_PROJ;
        set_project_dir(path, len);
    }
    else if (is_cplus_program(path, len) == true) {
        ProjectConfig.compile_obj_type = COMPILE_OBJ_TYPE_PROG;
        if (find_and_set_project_dir(path, len) != NULL) {
            return new_error("the .prog file is not in an valid cplus project directory.");
        }
    }
    else if (is_cplus_module(path, len) == true) {
        ProjectConfig.compile_obj_type = COMPILE_OBJ_TYPE_MOD;
        if (find_and_set_project_dir(path, len) != NULL) {
            return new_error("the .mod file is not in an valid cplus project directory.");
        }
    }
    else if (is_cplus_source(path, len) == true) {
        ProjectConfig.compile_obj_type = COMPILE_OBJ_TYPE_SRC;
    }
    else
        return new_error("invalid compile unit.");

    return NULL;
}

void projectConfigDestroy() {
//  mem_free(ProjectConfig.path_compiler);
//  mem_free(ProjectConfig.path_compile_obj);
    mem_free(ProjectConfig.path_stdmod);
//  mem_free(ProjectConfig.path_project);
//  mem_free(ProjectConfig.path_source);
//  mem_free(ProjectConfig.path_binary);
}
