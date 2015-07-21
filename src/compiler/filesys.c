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

bool fileInfoIsCplusModule(FileInfo* fileinfo) {
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
#ifdef PLATFORM_POSIX
    if ((directory->dir = opendir(dirpath)) == NULL) {
        return new_error("not found the directory.");
    }
    directory->dir_entry = readdir(directory->dir);
#endif

#ifdef PLATFORM_WINDOWS
    directory->file_data = (WIN32_FIND_DATA*)mem_alloc(sizeof(WIN32_FIND_DATA));
    if ((directory->h_search = FindFirstFile(dirpath, *(directory->file_data))) == INVALID_HANDLE_VALUE) {
        return new_error("not found the directory.")
    }
#endif

    return NULL;
}

error directoryGetNextFile(Directory* directory, FileInfo* fileinfo) {
#ifdef PLATFORM_POSIX
    if (directory->dir_entry == NULL) {
        return new_error("all files have been visited already.");
    }
    fileinfo->file_name = directory->dir_entry->d_name;

    switch (directory->dir_entry->d_type) {
    case DT_REG:
        fileinfo->file_type = FILE_TYPE_REGULAR;
        break;
    case DT_DIR:
        fileinfo->file_type = FILE_TYPE_DIR;
        break;
    default:
        fileinfo->file_type = FILE_TYPE_OTHER;
        break;
    }

    directory->dir_entry = readdir(directory->dir);
#endif

#ifdef PLATFORM_WINDOWS
    if (directory->file_data == NULL) {
        return new_error("all files have been visited already.");
    }
    fileinfo->file_name = directory->file_data->cFileName;

         if (directory->file_data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        fileinfo->file_type = FILE_TYPE_DIR;
    }
    else if (directory->file_data->dwFileAttributes & FILE_ATTRIBUTE_NORMAL) {
        fileinfo->file_type = FILE_TYPE_REGULAR;
    }
    else {
        fileinfo->file_type = FILE_TYPE_OTHER;
    }

    if (!FindNextFile(directory->h_search, *(directory->file_data))) {
        directory->file_data = NULL;
    }
#endif

    return NULL;
}

void directoryClose(Directory* directory) {
#ifdef PLATFORM_POSIX
    closedir(directory->dir);
#endif

#ifdef PLATFORM_WINDOWS
    mem_free(directory->find_data);
    FindClose(directory->h_search);
#endif
}
