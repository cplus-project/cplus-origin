/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "filesys.h"

error directoryOpen(Directory* directory, char* dirpath) {
    directory->head = NULL;
    directory->cur  = NULL;
    directory->tail = NULL;
    
#ifdef PLATFORM_POSIX
    DIR*           dir = opendir(dirpath);
    struct dirent* dir_entry;
    DirectoryFile* dir_node;

    if (dir == NULL) {
        return new_error("not found the directory.");
    }

    while ((dir_entry = readdir(dir)) != NULL) {
        dir_node = (DirectoryFile*)mem_alloc(sizeof(DirectoryFile));
        dir_node->next = NULL;
        dir_node->fileinfo.file_name = dir_entry->d_name;

        switch (dir_entry->d_type) {
            case DT_REG:
                dir_node->fileinfo.file_type = FILE_TYPE_REGULAR;
                break;
            case DT_DIR:
                dir_node->fileinfo.file_type = FILE_TYPE_DIR;
                break;
            default:
                dir_node->fileinfo.file_type = FILE_TYPE_OTHER;
                break;
        }

        if (directory->head != NULL) {
            directory->tail->next = dir_node;
            directory->tail = dir_node;
        }
        else {
            directory->head = dir_node;
            directory->cur  = dir_node;
            directory->tail = dir_node;
        }
    }
    closedir(dir);
#endif

#ifdef PLATFORM_WINDOWS
    
#endif
}

FileInfo* directoryGetNextFile(Directory* directory) {
    DirectoryFile* dir_node = directory->cur;
    if (dir_node == NULL) {
        return NULL;
    }
    directory->cur = directory->cur->next;
    return &(dir_node->fileinfo);
}

void directoryClose(Directory* directory) {
    for (;;) {
        if (directory->head == NULL) {
            directory->tail =  NULL;
            return;
        }
        directory->cur  = directory->head;
        directory->head = directory->head->next;
        mem_free(directory->cur);
    }
}
