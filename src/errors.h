/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The errors.h and errors.c provide the supports
 * of error handling.
 **/

#ifndef CPLUS_ERRORS_H
#define CPLUS_ERRORS_H

#include "common.h"

typedef struct error_table_node {
    char* tag;
    int16 tag_code;
}error_table_node;

typedef struct {

}error_table;

#endif
