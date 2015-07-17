/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "filesys.h"

/****** methods of FileInfo ******/

bool fileInfoIsCplusSrcFile(FileInfo* fileinfo) {
    int fn_len = strlen(fileinfo->file_name);
    if (fn_len < 6) {
        return false;
    }
    if (fileinfo->file_name[fn_len-6] != '.' || 
        fileinfo->file_name[fn_len-5] != 'c' ||
        fileinfo->file_name[fn_len-4] != 'p' ||
        fileinfo->file_name[fn_len-3] != 'l' ||
        fileinfo->file_name[fn_len-2] != 'u' ||
        fileinfo->file_name[fn_len-1] != 's' ){
        return false;
    }
    return true;
}

bool fileInfoIsCplusModular(FileInfo* fileinfo) {
    int fn_len = strlen(fileinfo->file_name);
    if (fn_len < 4) {
        return false;
    }
    if (fileinfo->file_name[fn_len-4] != '.' ||
        fileinfo->file_name[fn_len-3] != 'm' ||
        fileinfo->file_name[fn_len-2] != 'o' ||
        fileinfo->file_name[fn_len-1] != 'd' ){
        return false;
    }
    return true;
}

/****** methods of Directory  ******/

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
    DirectoryFile*  dir_node;
    WIN32_FIND_DATA file_data;
    HANDLE          h_search = FindFirstFile(dirpath, &file_data);

    if (h_search == INVALID_HANDLE_VALUE) {
        return new_error("not found the directory.");
    }

    do {
        dir_node = (DirectoryFile*)mem_alloc(sizeof(DirectoryFile));
        dir_node->next = NULL;
        dir_node->fileinfo.file_name = file_data.cFileName;

             if (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            dir_node->fileinfo.file_type = FILE_TYPE_DIR;
        }
        else if (file_data.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) {
            dir_node->fileinfo.file_type = FILE_TYPE_REGULAR;
        }
        else {
            dir_node->fileinfo.file_type = FILE_TYPE_OTHER;
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
    } while (FindNextFile(h_search, &file_data));

    FindClose(h_search);
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
