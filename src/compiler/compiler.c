/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "compiler.h"

error compilerInit(Compiler* compiler, ProjectConfig* projconf) {
    if (projconf == NULL) {
        return new_error("pass in the NULL project config.");
    }

    compiler->projconf = projconf;
    moduleScheduleQueueInit(&compiler->mod_schd_queue);
    moduleInfoDatabaseInit (&compiler->mod_info_db);

    // in the switch the compile object will be added into the ModuleScheduleQueue. the compile
    // object may be a cplus source file, some program directories or a module. because the
    // process for the source file and the program directories is the some with the modules, so
    // they will be treated as a module.
    //
    switch (compiler->projconf->compile_obj_type) {
    case COMPILE_OBJ_TYPE_PROJ:
        break;

    case COMPILE_OBJ_TYPE_PROG:
        break;

    case COMPILE_OBJ_TYPE_MOD:
        break;

    case COMPILE_OBJ_TYPE_SRC:
        break;

    default:
        return new_error("invalid compile object type.");
    }
}

error compilerBuild(Compiler* compiler) {
    return NULL;
}

error compilerRun(Compiler* compiler) {
    return NULL;
}

void compilerDestroy(Compiler* compiler) {

}
