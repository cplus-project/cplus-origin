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

#define FILE_TYPE_OTHER   0 // the file is which we don't care
#define FILE_TYPE_REGULAR 1 // the file is a regular file (maybe another source file)
#define FILE_TYPE_DIR     2 // the file is a directory    (maybe another moduler)

// the FileInfo is used to storage a file's information in
// a directory.
//
typedef struct {
    char* file_name;
    int8  file_type;
}FileInfo;

// when you want to estimate whether a file is the cplus source file or the modular,
// you should call fileInfoIsCplusSrcFile and fileInfoIsCplusModular methods based on
// the FileInfo objects' file_type property. write code like below:
//    ...
//    FileInfo* fileinfo;
//    ...
//    // do some operations here and now fileinfo is assigned with a valid address
//    switch (fileinfo->file_type) {
//    case FILE_TYPE_DIR:
//        if (fileInfoIsCplusModule(fileinfo) == true) {
//            // do some process for the modular
//        }
//        break;
//    case FILE_TYPE_REGULAR:
//        if (fileInfoIsCplusSrcFile(fileinfo) == true) {
//            // do some process for the source file
//        }
//        break;
//    default:
//        // some file we don't care...
//        break;
//    }
//    ...
//
extern bool fileInfoIsCplusSrcFile(FileInfo* fileinfo);
extern bool fileInfoIsCplusModule (FileInfo* fileinfo);

// the Directory represents a directory with a set of files
// inside it.
//
// example:
//    ...
//    Directory dir;
//    FileInfo  fileinfo;
//    error err = directoryOpen(&dir, "/usr/include");
//    if (err != NULL) {
//        // maybe the directory does not exist
//    }
//    while ((err = directoryGetNextFile(&dir, &fileinfo)) == NULL) {
//        switch (fileinfo.file_type) {
//        case FILE_TYPE_DIR:     // do the process if the file is a directory
//        case FILE_TYPE_REGULAR: // do the process if the file is a regular file
//        case FILE_TYPE_OTHER:   // do the process if the file is other type
//        }
//    }
//    directoryClose(&dir);
//    ...
//
typedef struct {
#ifdef PLATFORM_POSIX
    DIR*           dir;
    struct dirent* dir_entry;
#endif

#ifdef PLATFORM_WINDOWS
    HANDLE           h_search;
    WIN32_FIND_DATA* file_data;
#endif
}Directory;

extern error directoryOpen       (Directory* directory, char*     dirpath);
extern error directoryGetNextFile(Directory* directory, FileInfo* fileinfo);
extern void  directoryClose      (Directory* directory);

#endif
