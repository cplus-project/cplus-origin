/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The test for project.h and project.c.
 **/

#include "../project.h"

void create_temp_project_dir() {
    system("mkdir /tmp/cplus_project");
    system("mkdir /tmp/cplus_project/bin");
    system("mkdir /tmp/cplus_project/src");
    system("mkdir /tmp/cplus_project/src/test.mod");
    system("mkdir /tmp/cplus_project/src/test.prog");
    system("touch /tmp/cplus_project/src/test.prog/main.cplus");
}

void delete_temp_project_dir() {
    system("rm -rf /tmp/cplus_project");
}

void show_project_config(ProjectConfig* projconf) {
    printf("compiler path           : %s\r\n", projconf->path_compiler);
    printf("standard module path    : %s\r\n", projconf->path_stdmod);
    printf("project directory path  : %s\r\n", projconf->path_project);
    printf("project source directory: %s\r\n", projconf->path_source);
    printf("project binary directory: %s\r\n", projconf->path_binary);
    printf("compile target path     : %s\r\n", projconf->path_compile_obj);
    switch (projconf->compile_obj_type) {
    case COMPILE_OBJ_TYPE_PROJ:
        printf("compile target type     : project\r\n");
        break;
    case COMPILE_OBJ_TYPE_PROG:
        printf("compile target type     : program\r\n");
        break;
    case COMPILE_OBJ_TYPE_MOD:
        printf("compile target type     : module\r\n");
        break;
    case COMPILE_OBJ_TYPE_SRC:
        printf("compile target type     : source\r\n");
        break;
    default:
        printf("compile target type     : not valid\r\n");
        break;
    }
}

int main() {
    create_temp_project_dir();

    printf("when the compile target is a source file:\r\n");
    ProjectConfig projconf_src;
    projectConfigInit   (&projconf_src, "/usr/local/cplus-1.0/bin/cplus", "/tmp/cplus_project/src/test.prog/main.cplus");
    show_project_config (&projconf_src);
    projectConfigDestroy(&projconf_src);

    printf("\r\nwhen the compile target is a program directory:\r\n");
    ProjectConfig projconf_prog;
    projectConfigInit   (&projconf_prog, "/usr/local/cplus-1.0/bin/cplus", "/tmp/cplus_project/src/test.prog");
    show_project_config (&projconf_prog);
    projectConfigDestroy(&projconf_prog);

    printf("\r\nwhen the compile target is a module directory:\r\n");
    ProjectConfig projconf_mod;
    projectConfigInit   (&projconf_mod, "/usr/local/cplus-1.0/bin/cplus", "/tmp/cplus_project/src/test.mod");
    show_project_config (&projconf_mod);
    projectConfigDestroy(&projconf_mod);

    printf("\r\nwhen the compile target is a project directory:\r\n");
    ProjectConfig projconf_proj;
    projectConfigInit   (&projconf_proj, "/usr/local/cplus-1.0/bin/cplus", "/tmp/cplus_project");
    show_project_config (&projconf_proj);
    projectConfigDestroy(&projconf_proj);

    printf("\r\ntest the bad situation: ");
    ProjectConfig projconf_bad;
    if (projectConfigInit(&projconf_bad, "/usr/local/cplus-1.0/bin/cplus", "/tmp") != NULL) {
        debug("the path \"/tmp\" is invalid cplus project structure.");
    }

    delete_temp_project_dir();
    debug("\r\ntest over\r\n");
    return 0;
}
