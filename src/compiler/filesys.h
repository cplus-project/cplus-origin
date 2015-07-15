/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The filesys.h and filesys.c provide a set of functions
 * for operating files and directories and no need to care
 * about the differences between the operating systems.
 **/

#ifndef CPLUS_FILESYS_H
#define CPLUS_FILESYS_H

#include "common.h"

#ifdef PLATFORM_POSIX
    #include <dirent.h>
#endif

#ifdef PLATFORM_WINDOWS
    #include <windows.h>
#endif

#define FILE_TYPE_OTHER   0
#define FILE_TYPE_REGULAR 1
#define FILE_TYPE_DIR     2

// the FileInfo is used to storage a file's information in
// a directory.
//
typedef struct {
    char* file_name;
    int8  file_type;
}FileInfo;

typedef struct DirectoryFile {
    FileInfo fileinfo;
    struct DirectoryFile* next;
}DirectoryFile;

// the Directory represents a directory with a set of files
// inside it.
//
typedef struct {
    DirectoryFile* head;
    DirectoryFile* cur;
    DirectoryFile* tail;
}Directory;

extern error     directoryOpen       (Directory* directory, char* dirpath);
extern FileInfo* directoryGetNextFile(Directory* directory);
extern void      directoryClose      (Directory* directory);

#endif
