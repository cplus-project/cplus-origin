/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 * 
 *     the test for directory operations implemented in
 * filesys.h and filesys.c
 **/

#include "../filesys.h"

int main() {
    Directory dir;
    FileInfo  fileinfo;

#ifdef PLATFORM_POSIX
    error err = directoryOpen(&dir, "/usr/include");
#endif

#ifdef PLATFORM_WINDOWS
    error err = directoryOpen(&dir, "C:\\Windows\\System32");
#endif
    
    if (err != NULL) {
        debug(err);
        return 0;
    }

    while ((err = directoryGetNextFile(&dir, &fileinfo)) == NULL) {
        printf("file name is:%s\t", fileinfo.file_name);
        switch (fileinfo.file_type) {
        case FILE_TYPE_DIR:
            if (fileInfoIsCplusModule(&fileinfo) == true) {
                printf("file type is: cplus modular\r\n");
            }
            else {
                printf("file type is: directory\r\n");
            }
            break;
        case FILE_TYPE_REGULAR:
            if (fileInfoIsCplusSrcFile(&fileinfo) == true) {
                printf("file type is: cplus source file\r\n");
            }
            else {
                printf("file type is: regular file\r\n");
            }
            break;
        case FILE_TYPE_OTHER:
            printf("file type is: other\r\n");
            break;
        }
    }
    directoryClose(&dir);

    debug("test over...");
    return 0;
}