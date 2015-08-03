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

void show_project_config() {
    printf("compiler path           : %s\r\n", ProjectConfig.path_compiler);
    printf("standard module path    : %s\r\n", ProjectConfig.path_stdmod);
    printf("project directory path  : %s\r\n", ProjectConfig.path_project);
    printf("project source directory: %s\r\n", ProjectConfig.path_source);
    printf("project binary directory: %s\r\n", ProjectConfig.path_binary);
    printf("compile target path     : %s\r\n", ProjectConfig.path_compile_obj);
    switch (ProjectConfig.compile_obj_type) {
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
    projectConfigInit("/usr/local/cplus-1.0/bin/cplus", "/tmp/cplus_project/src/test.prog/main.cplus");
    show_project_config();
    
    printf("\r\nwhen the compile target is a program directory:\r\n");
    projectConfigInit("/usr/local/cplus-1.0/bin/cplus", "/tmp/cplus_project/src/test.prog");
    show_project_config();
    
    printf("\r\nwhen the compile target is a module directory:\r\n");
    projectConfigInit("/usr/local/cplus-1.0/bin/cplus", "/tmp/cplus_project/src/test.mod");
    show_project_config();
    
    printf("\r\nwhen the compile target is a project directory:\r\n");
    projectConfigInit("/usr/local/cplus-1.0/bin/cplus", "/tmp/cplus_project");
    show_project_config();
    
    printf("\r\ntest the bad situation: ");
    if (projectConfigInit("/usr/local/cplus-1.0/bin/cplus", "/tmp") != NULL) {
        debug("the path \"/tmp\" not valid cplus project structure.");
    }
    
    projectConfigDestroy();
    delete_temp_project_dir();
    
    debug("\r\ntest over\r\n");
    return 0;
}
