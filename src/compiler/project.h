/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The project.h and project.c provide some datastructs
 * and functions to process the information about the project.
 **/

#ifndef CPLUS_PROJECT_H
#define CPLUS_PROJECT_H

#include <unistd.h>
#include "common.h"
#include "dynamicarr.h"
#include "path.h"

// The ProjectConfig is used to save some information about the project.
//
typedef struct {
    // the path of the compiler
    char* path_compiler;
    int   path_compiler_len;
    
    // the path of the standard modules
    char* path_stdmods;
    int   path_stdmods_len;
    
    // the path of the module passed to the compiler as the parameter
    char* path_buildmod;
    int   path_buildmod_len;
    
    // the path of the project
    char* path_project;
    int   path_project_len;

    // the path of the source directory
    char* path_srcdir;
    int   path_srcdir_len;

    // the path of the binary directory
    char* path_bindir;
    int   path_bindir_len;
}ProjectConfig;

extern error projectConfigInit   (ProjectConfig* projconf, char* path_compiler, char* path_buildmod);
extern void  projectConfigDestroy(ProjectConfig* projconf);

#endif
