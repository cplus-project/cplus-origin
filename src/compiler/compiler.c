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
    case COMPILE_OBJ_TYPE_PROJ: {
            Module*        mod;
            DIR*           dir;
            struct dirent* dir_entry;
            int            len;
            DynamicArrChar darr;
            dynamicArrCharInit(&darr, 255);
            // trival the 'project/src' directory and add all xxx.prog directories into the
            // module set.
            //
            if ((dir = opendir(projconf->path_source)) == NULL) {
                return new_error("open project source directory failed.");
            }
            while ((dir_entry = readdir(dir)) != NULL) {
                len = strlen(dir_entry->d_name);
                if (dir_entry->d_type == DT_DIR && /*moduleSchedulerIsProgDir(dir_entry->d_name, len) == true*/) {
                    dynamicArrCharAppend (&darr, projconf->path_source, projconf->srcdir_path_len);
                    dynamicArrCharAppendc(&darr, '/');
                    dynamicArrCharAppend (&darr, dir_entry->d_name, len);
                    
                    mod = (Module*)mem_alloc(sizeof(Module));
                    mod->mod_info           = (ModuleInfo*)mem_alloc(sizeof(ModuleInfo*));
                    mod->mod_path           = dynamicArrCharGetStr(&darr);
                    mod->mod_path_len       = darr.used;
                    mod->is_main            = true;
                    mod->dependences_parsed = false;
                    mod->srcfiles           = // TODO: get file list
                    mod->mod_info->mod_name     = dir_entry->d_name;
                    mod->mod_info->compile_over = false;
                    mod->mod_info->id_table     = NULL;

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
            mod->mod_info           = (ModuleInfo*)mem_alloc(sizeof(ModuleInfo));
            mod->mod_path           = projconf->path_compile_obj;
            mod->mod_path_len       = projconf->cplobj_path_len;
            mod->is_main            = true;
            mod->dependences_parsed = false;
            mod->srcfiles           = // TODO: get file list
            mod->mod_info->mod_name     = path_last(projconf->path_compile_obj, projconf->cplobj_path_len);
            mod->mod_info->compile_over = false;
            mod->mod_info->id_table     = NULL;
            moduleScheduleQueueAddMod(&compiler->mod_schd_queue, mod);
            return NULL;
        }

    case COMPILE_OBJ_TYPE_MOD: {
            Module* mod = (Module*)mem_alloc(sizeof(Module));
            mod->mod_info           = (ModuleInfo*)mem_alloc(sizeof(ModuleInfo));
            mod->mod_path           = projconf->path_compile_obj;
            mod->mod_path_len       = projconf->cplobj_path_len;
            mod->is_main            = false;
            mod->dependences_parsed = false;
            mod->srcfiles           = // TODO: get file list
            mod->mod_info->mod_name     = // TODO: get module name by path
            mod->mod_info->compile_over = false;
            mod->mod_info->id_table     = NULL;
            moduleInfoDatabaseAdd    (&compiler->mod_info_db   , mod->mod_info);
            moduleScheduleQueueAddMod(&compiler->mod_schd_queue, mod);
            return NULL;
        }

    case COMPILE_OBJ_TYPE_SRC: {
            Module* mod = (Module*)mem_alloc(sizeof(Module));
            mod->mod_info           = (ModuleInfo*)mem_alloc(sizeof(ModuleInfo));
            mod->mod_path           = projconf->path_source;
            mod->mod_path_len       = projconf->srcdir_path_len;
            mod->is_main            = true;
            mod->dependences_parsed = false;
            mod->srcfiles           = (SourceFile*)mem_alloc(sizeof(SourceFile));
            mod->mod_info->mod_name      = path_last(projconf->path_compile_obj, projconf->cplobj_path_len);
            mod->mod_info->compile_over  = false;
            mod->mod_info->id_table      = NULL;
            mod->srcfiles->file_name     = mod->mod_info->mod_name;
            mod->srcfiles->file_name_len = strlen(mod->srcfiles->file_name);
            mod->srcfiles->next          = NULL;
            moduleScheduleQueueAddMod(&compiler->mod_schd_queue, mod);
            return NULL;
        }

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
