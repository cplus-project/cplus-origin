/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "compiler.h"

error compilerInit(Compiler* compiler, ProjectConfig* projconf) {
    compiler->project_config = projconf;
    moduleSchedulerInit(&compiler->mod_scheduler, projconf);
}

error compilerBuild(Compiler* compiler) {
    return NULL;
}

error compilerRun(Compiler* compiler) {
    return NULL;
}

void compilerDestroy(Compiler* compiler) {

}
