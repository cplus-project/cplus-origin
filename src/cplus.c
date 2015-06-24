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
#include "parser.h"

// command:
//   build    build the specific C+ project
//   run      build and run the specific C+ project
//   test     test the specific C+ file or project
//   help     display the manual
int main(int argc, char* argv[]) {
    error err = NULL;

    Parser parser;
    parserInit (&parser);
    parserStart(&parser, "./input/main.cplus");

    return 0;
}

