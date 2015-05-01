/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The errsys.h and errsys.c implement the error system
 * of the C+ programming language. The error system is a
 * runtime system similar to the exception handling in other
 * language.
 **/

#ifndef CPLUS_ERRSYS_H
#define CPLUS_ERRSYS_H

#include "common.h"

#define NODE_COLOR_RED   0x00
#define NODE_COLOR_BLACK 0x01

#define NODE_CMP_LT      0x00
#define NODE_CMP_EQ      0x01
#define NODE_CMP_GT      0x02

typedef struct {
    bool  must_deal; // is the error must be dealt
    char* file_name; // which file     the error come from
    char* func_name; // which function the error come from
    int32 line;      // which line     the error come from
    int16 col;       // which column   the error come from
}err_info;

typedef struct err_info_log_node {
    err_info info;
    struct err_info_log_node* next;
}err_info_log_node;

// the err_info_log is used to log a set the errors' must-flags and positions.
typedef struct {
    err_info_log_node* head;
    err_info_log_node* tail;
}err_info_log;

static void err_info_log_init   (err_info_log* log);
static void err_info_log_add    (err_info_log* log, bool must, char* file_name, char* func_name, int32 line, int16 col);
static void err_info_log_clear  (err_info_log* log);
static void err_info_log_destroy(err_info_log* log);

// errors with the same error tag and be detected in the same function(the error's
// scope is based on the function-body-braces) will be saved in the same one error
// entry.
typedef struct {
    char*        err_tag;
    int16        err_count;  // the total of the errors in this entry
    int16        must_count; // the total of the errors which set the must-flag as true in this entry
    err_info_log info_log;   // the log of errors with the same error tag but the with the different position
    bool         is_dealt;   // flag that is the error dealt yet
}err_entry;

typedef struct err_table_node {
    err_entry err;
    int8      color;
    struct err_table_node* parent;
    struct err_table_node* lchild;
    struct err_table_node* rchild;
}err_table_node;

// the err_table is used to storage a set of errors based on the error tag.
typedef struct {
    err_table_node* root;
    int16           err_count;
    int16           must_count;
}err_table;

extern void  err_table_init   (err_table* etab);
extern error err_table_add    (err_table* etab, char* err_tag, bool must, char* file_name, char* func_name, int32 line, int16 col);
extern bool  err_table_rm     (err_table* etab, char* err_tag);
extern bool  err_table_rm_wdcd(err_table* etab, char* err_tag_wdcd);
extern void  err_table_destroy(err_table* etab);

#endif
