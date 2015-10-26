/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "project.h"

error projectConfigInit(ProjectConfig* projconf, char* path_compiler, char* path_buildmod) {
    char*          path;
    int            path_len;
    DynamicArrChar darr;
    dynamicArrCharInit(&darr, 255);
    
    projconf->path_compiler = path_compiler;
    projconf->path_compiler_len = strlen(path_compiler);

    projconf->path_buildmod = path_buildmod;
    projconf->path_buildmod_len = strlen(path_buildmod);
    
    if (path_isabs(projconf->path_buildmod, projconf->path_buildmod_len) == false) {
        if ((path = getcwd(NULL, 0)) == NULL) {
            return new_error("get current work path failed.");
        }
        dynamicArrCharAppend (&darr, path, strlen(path));
        dynamicArrCharAppendc(&darr, path_separator);
        dynamicArrCharAppend (&darr, projconf->path_buildmod, projconf->path_buildmod_len);
        projconf->path_buildmod = dynamicArrCharGetStr(&darr);
        projconf->path_buildmod_len = darr.used;

        mem_free(path);
        dynamicArrCharClear(&darr);
    }
}

void projectConfigDestroy(ProjectConfig* projconf) {
//  mem_free(projconf->path_compiler);
//  mem_free(projconf->path_compile_obj);
    mem_free(projconf->path_stdmod);
//  mem_free(projconf->path_project);
//  mem_free(projconf->path_source);
//  mem_free(projconf->path_binary);
}
