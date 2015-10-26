/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The path.h and path.c provide some functions to
 * process one path.
 **/

#ifndef CPLUS_PATH_H
#define CPLUS_PATH_H

#include "common.h"
#include "sys/stat.h"
#include "unistd.h"

#ifdef PLATFORM_WINDOWS
    const char path_separator = '\\';
#else
    const char path_separator = '/';
#endif

// note:
//    be careful that the return strings of path_prev() and path_last()
//    uses mem_alloc(), so never forget to call mem_free() to release the
//    memory of these return strings.
//
extern char* path_prev (const char* path, int path_len);
extern char* path_last (const char* path, int path_len);
extern bool  path_isabs(const char* path, int path_len);
extern error path_isdir(const char* path);
extern error path_isreg(const char* path);

#endif
