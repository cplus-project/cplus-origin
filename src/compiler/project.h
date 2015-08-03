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

#include "common.h"
#include "dynamicarr.h"
#include "path.h"

// all functions listed below will return true if the specified path is the one
// you want to judge.
//
extern bool is_cplus_source (char* path, int path_len);
extern bool is_cplus_module (char* path, int path_len);
extern bool is_cplus_program(char* path, int path_len);
extern bool is_cplus_project(char* path, int path_len);

#define COMPILE_OBJ_TYPE_PROJ 0
#define COMPILE_OBJ_TYPE_PROG 1
#define COMPILE_OBJ_TYPE_MOD  2
#define COMPILE_OBJ_TYPE_SRC  3

// you can use the ProjectConfig just like a global static class.
//
// note:
//    the ProjectConfig should be the unique global variable.
//
struct {
    char* path_compiler;    // the path of the cplus compiler program.
    char* path_stdmod;      // the path of the standard modules.
    char* path_project;     // the path of the current building project's directory.
    char* path_binary;      // the path of the current building project's binary files.
    char* path_source;      // the path of the current building project's source files.
    char* path_compile_obj; // the path of the compile object(which is passed to the compiler as a parameter)

    int8  compile_obj_type; // one of the file types in .cplus/.prog/.mod/project
}ProjectConfig;

extern error projectConfigInit   (char* compiler_path, char* compile_obj_path);
extern void  projectConfigDestroy();

#endif
