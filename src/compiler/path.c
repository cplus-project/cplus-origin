/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "path.h"

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
char* path_prev(const char* path, int path_len) {
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
char* path_last(const char* path, int path_len) {
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
error path_isdir(const char* path) {
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
error path_isreg(const char* path) {
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

// return true if the path is an absolute path.
//
bool path_isabs(const char* path, int path_len) {
    if (path == NULL) {
        return false;
    }
#ifdef PLATFORM_WINDOWS
    if (path_len > 2 && ('A' <= path[0] && path[0] <= 'Z' || 'a' <= path[0] && path[0] <= 'z') && path[1] == ':' && path[2] == '\\')
        return true;
#else
    if (path_len > 0 && path[0] == '/')
        return true;
#endif
    return false;
}
