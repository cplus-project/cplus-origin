/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The test for module.h and module.c
 **/

#include "../module.h"

static void moduleInfoDatabaseShow(ModuleInfoDatabaseNode* node) {
    if (node != NULL) {
        if (node->lchild != NULL) {
            moduleInfoDatabaseShow(node->lchild);
        }
        printf("%s ", node->mod_info->mod_name);
        if (node->rchild != NULL) {
            moduleInfoDatabaseShow(node->rchild);
        }
    }
}

static void moduleScheduleQueueShow(ModuleScheduleQueue* queue) {
    ModuleScheduleQueueNode* ptr;
    int cur_pos_count = 8;
    int cur_pos;
    printf("[NODES] ");
    for (ptr = queue->head->next; ptr != NULL; ptr = ptr->next) {
        if (strcmp(ptr->mod->mod_info->mod_name, queue->cur->mod->mod_info->mod_name) != 0) {
            cur_pos_count += 3;
        } else {
            cur_pos = cur_pos_count;
        }
        printf("%s ", ptr->mod->mod_info->mod_name);
    }
    printf("\r\n");
    for (cur_pos_count = 0; cur_pos_count < cur_pos; cur_pos_count++) {
        printf(" ");
    }
    printf("^\r\n");
}

int main() {
    printf("****** test ModuleInfoDatabase ******\r\n\r\n");

    int i;
    error err = NULL;
    ModuleInfo mod_infos[8];
    mod_infos[0].mod_name = "m1";
    mod_infos[1].mod_name = "m2";
    mod_infos[2].mod_name = "m3";
    mod_infos[3].mod_name = "m4";
    mod_infos[4].mod_name = "m5";
    mod_infos[5].mod_name = "m6";
    mod_infos[6].mod_name = "m7";
    mod_infos[7].mod_name = "m8";

    ModuleInfoDatabase mod_info_db;
    moduleInfoDatabaseInit(&mod_info_db);
    for (i = 0; i < 3; i++) {
        mod_infos[i].compile_over = false;
        mod_infos[i].id_table     = NULL;
        if ((err = moduleInfoDatabaseAdd(&mod_info_db, &mod_infos[i])) != NULL) {
            debug(err);
        }
    }
    printf("all node in the ModuleInfoDatabase: ");
    moduleInfoDatabaseShow(mod_info_db.root);
    printf("\r\n\r\n");
    
    printf("now try to test the exist function(test the m3): ");
    moduleInfoDatabaseExist(&mod_info_db, "m3") == true ? printf("[m3 exists]\r\n\r\n") : printf("[test failed]\r\n\r\n");

    ModuleInfo* mod_info;
    printf("now try to get the m4 and m1: ");
    if ((mod_info = moduleInfoDatabaseGet(&mod_info_db, "m4")) == NULL) {
        printf("[get m4 failed] ");
    } else {
        printf("[get m4 success] ");
    }
    if ((mod_info = moduleInfoDatabaseGet(&mod_info_db, "m1")) == NULL) {
        printf("[get m1 failed]\r\n\r\n");
    } else {
        printf("[get m1 success]\r\n\r\n");
    }

    printf("\r\n****** test ModuleScheduleQueue ******\r\n\r\n");

    ModuleScheduleQueue mod_schd_queue;
    moduleScheduleQueueInit(&mod_schd_queue);
    printf("test the isempty function, now the queue should be empty, right? ");
    moduleScheduleQueueIsEmpty(&mod_schd_queue) == true ? printf("[YES]\r\n\r\n") : printf("[test failed]\r\n\r\n");

    Module mods[8];
    for (i = 0; i < 3; i++) {
        if ((err = moduleInfoDatabaseAdd(&mod_info_db, &mod_infos[i])) != NULL) {
            printf("add the module %s failed: err: %s\r\n", mod_infos[i].mod_name, err);
            continue;
        } else {
            printf("test failed once...\r\n");
        }
    }
    for (i = 0; i < 8; i++) {
        mods[i].mod_info           = &mod_infos[i];
        mods[i].mod_path           = NULL;
        mods[i].mod_path_len       = 0;
        mods[i].is_main            = false;
        mods[i].dependences_parsed = false;
        mods[i].srcfiles           = NULL;
    }

    printf("\r\nnow we will test the add function of the ModuleScheduleQueue, we add the m4 and call get function:\r\n");
    moduleScheduleQueueAddMod(&mod_schd_queue, &mods[3]);
    moduleScheduleQueueGetHeadMod(&mod_schd_queue);
    moduleScheduleQueueShow(&mod_schd_queue);

    printf("next we add the m5 and m6, and then call get function:\r\n");
    moduleScheduleQueueAddMod(&mod_schd_queue, &mods[4]);
    moduleScheduleQueueAddMod(&mod_schd_queue, &mods[5]);
    moduleScheduleQueueShow(&mod_schd_queue);
    moduleScheduleQueueGetHeadMod(&mod_schd_queue);
    moduleScheduleQueueShow(&mod_schd_queue);

    printf("next we add the m7 and m8, and then call get function:\r\n");
    moduleScheduleQueueAddMod(&mod_schd_queue, &mods[6]);
    moduleScheduleQueueAddMod(&mod_schd_queue, &mods[7]);
    moduleScheduleQueueShow(&mod_schd_queue);
    moduleScheduleQueueGetHeadMod(&mod_schd_queue);
    moduleScheduleQueueShow(&mod_schd_queue);
    
    printf("\r\nnow test the get function: ");
    Module* mod = moduleScheduleQueueGetHeadMod(&mod_schd_queue);
    if (mod == NULL) {
        printf("[test failed]\r\n\r\n");
    } else {
        printf("[the name of the mod is %s]\r\n\r\n", mod->mod_info->mod_name);
    }
    
    printf("now we test the delete function, try to delete twice and now the nodes in the queue is:\r\n");
    moduleScheduleQueueDelHeadMod(&mod_schd_queue);
    moduleScheduleQueueDelHeadMod(&mod_schd_queue);
    moduleScheduleQueueShow(&mod_schd_queue);
    
    printf("\r\nnow we delete all nodes in the queue and check whether the queue is empty: ");
    moduleScheduleQueueDelHeadMod(&mod_schd_queue);
    moduleScheduleQueueDelHeadMod(&mod_schd_queue);
    moduleScheduleQueueDelHeadMod(&mod_schd_queue);
    moduleScheduleQueueIsEmpty(&mod_schd_queue) == true ? printf("[is empty]\r\n\r\n") : printf("[test failed]\r\n\r\n");

    debug("test over");
    return 0;
}
