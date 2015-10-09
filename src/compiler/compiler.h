/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The compiler.h and compiler.c implement the Compiler.
 * Compiler is used to compile the project based on the
 * configuration.
 **/

#ifndef CPLUS_COMPILER_H
#define CPLUS_COMPILER_H

#include "common.h"
#include "project.h"
#include "module.h"

typedef struct {
    ProjectConfig*  project_config;
}Compiler;

extern error compilerInit   (Compiler* compiler, ProjectConfig* projconf);
extern error compilerBuild  (Compiler* compiler);
extern error compilerRun    (Compiler* compiler);
extern void  compilerDestroy(Compiler* compiler);

#endif
