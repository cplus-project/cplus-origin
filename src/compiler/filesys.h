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

extern 

#endif
