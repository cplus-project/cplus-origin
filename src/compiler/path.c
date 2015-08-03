/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "path.h"

#ifdef PLATFORM_WINDOWS
    const char path_separator = '\\';
#else
    const char path_separator = '/';
#endif

// get the parent level of the target path. return NULL if the target
// path does not have the parent level.
//
// example:
//   [unix-like]
//     1. path "/home/user/src" will return '/home/user'
//     2. path "/home" will return "/"
//     3. path "/" will return NULL
//
//   [windows]
//     1. path "C:\Windows\System32" will return "C:\Windows"
//     1. path "C:\Windows" will return "C:"
//     2. path "C:" will return NULL
//
char* path_prev(char* path, int path_len) {
    if (path == NULL || strcmp(path, "/") == 0) {
        return NULL;
    }
    int i;
    for (i = path_len-1; i >= 0; i--) {
        if (i == 0) {
            return path[0] == '/' ? "/" : NULL;
        }
        if (path[i] == path_separator) {
            break;
        }
    }
    char* path_ret = (char*)mem_alloc(sizeof(char)*i);
    int j;
    for (j = 0; j < i; j++) {
        path_ret[j] = path[j];
    }
    path_ret[i] = '\0';

    return path_ret;
}

// get the last level of the target path.
//
// example:
//    path "/usr/local/bin" will return "bin"
//
char* path_last(char* path, int path_len) {
    if (path == NULL) {
        return NULL;
    }
    if (strcmp(path, "/") == 0) {
        return "/";
    }
    int i, j;
    for (i = path_len-1; i >= 0; i--) {
        if (path[i] == path_separator) {
            break;
        }
    }
    char* path_ret = (char*)mem_alloc(sizeof(char) * (path_len-i));
    j = 0;
    for (i += 1; i < path_len; i++) {
        path_ret[j] = path[i];
        j++;
    }
    path_ret[j] = '\0';

    return path_ret;
}

// return NULL if the target is a directory.
//
error path_isdir(char* path) {
    if (path == NULL) {
        return new_error("the path should not be NULL.");
    }
    struct stat st;
    if (stat(path, &st) != 0) {
        return new_error("get the path's details failed.");
    }
    if (S_ISDIR(st.st_mode)) {
        return NULL;
    }
    return new_error("the path is not a directory.");
}

// return NULL if the target is a regular file.
//
error path_isreg(char* path) {
    if (path == NULL) {
        return new_error("the path should not be NULL.");
    }
    struct stat st;
    if (stat(path, &st) != 0) {
        return new_error("get the path's details failed.");
    }
    if (S_ISREG(st.st_mode)) {
        return NULL;
    }
    return new_error("the path is not a regular file.");
}
