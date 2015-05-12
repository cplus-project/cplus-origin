/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The fileset.h and fileset.c provide some struct
 * to storage and organize a set of files.
 **/

#ifndef CPLUS_FILESET_H
#define CPLUS_FILESET_H

#include "common.h"

typedef struct file_queue_node {
    char* file_name;
    struct file_queue_node* next;
}file_queue_node;

// a queue struct to storage files.
typedef struct {
    file_queue_node* head;
    file_queue_node* tail;
}file_queue;

extern void  file_queue_init   (file_queue* fqueue);
extern void  file_queue_enqueue(file_queue* fqueue, char* file_name);
extern bool  file_queue_exist  (file_queue* fqueue, char* file_name);
extern char* file_queue_front  (file_queue* fqueue);
extern error file_queue_dequeue(file_queue* fqueue);
extern void  file_queue_destroy(file_queue* fqueue);

typedef struct file_stack_node {
    char* file_name;
    struct file_stack_node* next;
}file_stack_node;

// a stack struct to storage files;
typedef struct {
    file_stack_node* top;
}file_stack;

extern void  file_stack_init   (file_stack* fstk);
extern void  file_stack_push   (file_stack* fstk, char* file_name);
extern bool  file_stack_isempty(file_stack* fstk);
extern char* file_stack_top    (file_stack* fstk);
extern void  file_stack_pop    (file_stack* fstk);
extern void  file_stack_destroy(file_stack* fstk);

#define NODE_COLOR_RED   0x00
#define NODE_COLOR_BLACK 0x01

#define NODE_CMP_LT      0x00
#define NODE_CMP_EQ      0x01
#define NODE_CMP_GT      0x02

typedef struct file_tree_node {
    char* file_name;
    int8  color;
    struct file_tree_node* parent;
    struct file_tree_node* lchild;
    struct file_tree_node* rchild;
}file_tree_node;

// a tree struct to storage files.
typedef struct {
    file_tree_node* root;
}file_tree;

extern void  file_tree_init   (file_tree* ftree);
extern error file_tree_add    (file_tree* ftree, char* file_name);
extern bool  file_tree_exist  (file_tree* ftree, char* file_name);
extern void  file_tree_destroy(file_tree* ftree);

#endif
