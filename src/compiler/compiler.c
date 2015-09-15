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
    error err = NULL;
    switch (compiler->projconf->compile_obj_type) {
    case COMPILE_OBJ_TYPE_PROJ: {
            Module*        mod;
            DIR*           dir;
            struct dirent* dir_entry;
            int            len;
            DynamicArrChar darr;
            dynamicArrCharInit(&darr, 255);
            // trival the 'project/src' directory and add all xxx.prog directories into the
            // module schedule queue.
            //
            if ((dir = opendir(projconf->path_source)) == NULL) {
                return new_error("open project source directory failed.");
            }
            while ((dir_entry = readdir(dir)) != NULL) {
                len = strlen(dir_entry->d_name);
                if (dir_entry->d_type == DT_DIR &&
                    len > 5 &&
                    dir_entry->d_name[len-5] == '.' &&
                    dir_entry->d_name[len-4] == 'p' &&
                    dir_entry->d_name[len-3] == 'r' &&
                    dir_entry->d_name[len-2] == 'o' &&
                    dir_entry->d_name[len-1] == 'g' ){
                    dynamicArrCharAppend (&darr, projconf->path_source, projconf->srcdir_path_len);
                    dynamicArrCharAppendc(&darr, '/');
                    dynamicArrCharAppend (&darr, dir_entry->d_name, len);

                    mod = (Module*)mem_alloc(sizeof(Module));
                    if ((err = moduleInitByPath(mod, dynamicArrCharGetStr(&darr), darr.used, projconf)) != NULL) {
                        return new_error(err);
                    }

                    moduleScheduleQueueAddMod    (&compiler->mod_schd_queue, mod);
                    moduleScheduleQueueGetHeadMod(&compiler->mod_schd_queue);
                    dynamicArrCharClear(&darr);
                }
            }
            closedir(dir);
            dynamicArrCharDestroy(&darr);
            return NULL;
        }

    case COMPILE_OBJ_TYPE_PROG: {
            Module* mod = (Module*)mem_alloc(sizeof(Module));
            if ((err = moduleInitByPath(mod, projconf->path_compile_obj, projconf->cplobj_path_len, projconf)) != NULL) {
                return new_error(err);
            }
            moduleScheduleQueueAddMod(&compiler->mod_schd_queue, mod);
            return NULL;
        }

    case COMPILE_OBJ_TYPE_MOD: {
            Module* mod = (Module*)mem_alloc(sizeof(Module));
            if ((err = moduleInitByPath(mod, projconf->path_compile_obj, projconf->cplobj_path_len, projconf)) != NULL) {
                return new_error(err);
            }
            moduleInfoDatabaseAdd    (&compiler->mod_info_db   , mod->mod_info);
            moduleScheduleQueueAddMod(&compiler->mod_schd_queue, mod);
            return NULL;
        }

    case COMPILE_OBJ_TYPE_SRC: {
            Module* mod = (Module*)mem_alloc(sizeof(Module));
            if ((err = moduleInitByPath(mod, projconf->path_compile_obj, projconf->cplobj_path_len, projconf)) != NULL) {
                return new_error(err);
            }
            moduleScheduleQueueAddMod(&compiler->mod_schd_queue, mod);
            return NULL;
        }

    default:
        return new_error("invalid compile object type.");
    }
}

error compilerBuild(Compiler* compiler) {
    Module* mod;
    for (;;) {
        mod = moduleScheduleQueueGetHeadMod(&compiler->mod_schd_queue);
        if (mod == NULL) {
            break;
        }
        moduleDisplayDetails(mod);
        moduleScheduleQueueDelHeadMod(&compiler->mod_schd_queue);
    }
    return NULL;
}

error compilerRun(Compiler* compiler) {
    return NULL;
}

void compilerDestroy(Compiler* compiler) {

}
