/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     This file is the main function file. it organizes
 * and composes all other functions needed by the compiler.
 **/

#include <string.h>
#include "common.h"
#include "compiler.h"
#include "project.h"
#include "parser.h"

// command:
//   build    build the specific cplus project
//   run      build and run the specific cplus project
//   test     test the specific cplus file or project
//   format   adjust the indent of the program
//   help     display the manual
//
int main(int argc, char* argv[]) {
    error err;
    ProjectConfig projconf;
    err = projectConfigInit(&projconf, argv[0], "/home/jikai/c_projects/test/CplusProject");
    if (err != NULL) {
        debug(err);
    }
    Compiler compiler;
    err = compilerInit(&compiler, &projconf);
    if (err != NULL) {
        debug(err);
    }
    compilerBuild  (&compiler);
    compilerDestroy(&compiler);
    
    projectConfigDestroy(&projconf);
    return 0;
}

