/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "dynamicarr.h"

error dynamicArrCharInit(DynamicArrChar* darr, int64 capacity) {
    if (capacity <= 0) {
        return new_error("err: the capacity of the dynamic char array should be a positive number!");
    }

    darr->total_cap = capacity;
    darr->used      = 0;

    // the darr->first is the first buffer of the dynamic char array. this buffer's
    // capacity is the same as the parameter passed as 'capacity'. if requirement of
    // size is bigger than the darr->total_cap, then a new buffer will created and
    // be linked with the darr->next pointer.
    //
    darr->first       = (DynamicArrCharNode*)mem_alloc(sizeof(DynamicArrCharNode));
    darr->first->cap  = capacity;
    darr->first->i    = 0;
    darr->first->arr  = (char*)mem_alloc(sizeof(char) * capacity);
    darr->first->next = NULL;

    darr->cur = darr->first;
    return NULL;
}

void dynamicArrCharAppend(DynamicArrChar* darr, char* str, int64 len) {
    int64 j    = 0;
    int64 need = darr->used + len;

    if (need <= darr->total_cap) {
        for (j = 0; j < len; j++) {
            darr->cur->arr[darr->cur->i] = str[j];
            darr->cur->i++;
            darr->used++;
        }
    }
    else {
        int cur_left_cap = darr->total_cap - darr->used;
        int new_node_cap = darr->total_cap;
        
        // like most of the dynamic array, when the capacity needed is more than the space
        // left of the current array, a new node will be created and its size will be expanded
        // as twice of its current size(its size is equal to the array's total capacity
        // at first) every time.
        //
        while (new_node_cap + darr->total_cap < need) {
            new_node_cap *= 2;
        }

        darr->cur->next = (DynamicArrCharNode*)mem_alloc(sizeof(DynamicArrCharNode));
        darr->cur->next->cap  = new_node_cap;
        darr->cur->next->i    = 0;
        darr->cur->next->arr  = (char*)mem_alloc(sizeof(char) * new_node_cap);
        darr->cur->next->next = NULL;
        darr->total_cap += new_node_cap;

        for (j = 0; j < cur_left_cap; j++) {
            darr->cur->arr[darr->cur->i] = str[j];
            darr->cur->i++;
            darr->used++;
        }
        darr->cur = darr->cur->next;
        for (; j < len; j++) {
            darr->cur->arr[darr->cur->i] = str[j];
            darr->cur->i++;
            darr->used++;
        }
    }
}

void dynamicArrCharAppendc(DynamicArrChar* darr, char ch) {
    if (darr->cur->i >= darr->cur->cap) {
        darr->cur->next = (DynamicArrCharNode*)mem_alloc(sizeof(DynamicArrCharNode));
        darr->cur->next->cap  = darr->total_cap;
        darr->cur->next->i    = 0;
        darr->cur->next->arr  = (char*)mem_alloc(sizeof(char) * darr->total_cap);
        darr->cur->next->next = NULL;
        darr->total_cap *= 2;
        darr->cur = darr->cur->next;
    }
    darr->cur->arr[darr->cur->i] = ch;
    darr->cur->i++;
    darr->used++;
}

void dynamicArrCharAppendDarr(DynamicArrChar* darr, DynamicArrChar* darr_src) {
    int64               j    = 0;
    int64               need = darr->used + darr_src->used;
    DynamicArrCharNode* node = NULL;

    if (need <= darr->total_cap) {
        for (node = darr_src->first; node != NULL; node = node->next) {
            for (j = 0; j < node->i; j++) {
                darr->cur->arr[darr->cur->i] = node->arr[j];
                darr->cur->i++;
                darr->used++;
            }
        }
    }
    else {
        int new_node_cap = darr->total_cap;

        // like most of the dynamic array, when the capacity needed is more than the space
        // left of the current array, a new node will be created and its size will be expanded
        // as twice of its current size(its size is equal to the array's total capacity
        // at first) every time.
        //
        while (new_node_cap + darr->total_cap < need) {
            new_node_cap *= 2;
        }

        darr->cur->next = (DynamicArrCharNode*)mem_alloc(sizeof(DynamicArrCharNode));
        darr->cur->next->cap  = new_node_cap;
        darr->cur->next->i    = 0;
        darr->cur->next->arr  = (char*)mem_alloc(sizeof(char) * new_node_cap);
        darr->cur->next->next = NULL;
        darr->total_cap += new_node_cap;

        for (node = darr_src->first; node != NULL; node = node->next) {
            for (j = 0; j < node->i; j++) {
                darr->cur->arr[darr->cur->i] = node->arr[j];
                darr->cur->i++;
                darr->used++;
                if (darr->cur->i >= darr->cur->cap) {
                    darr->cur = darr->cur->next;
                }
            }
        }
    }
}

// return true if the given string is equal to the content
// in the dynamic char array.
//
bool dynamicArrCharEqual(DynamicArrChar* darr, char* str, int64 len) {
    if (darr->used != len) {
        return false;
    }
    int64 i;
    int64 j = 0;
    DynamicArrCharNode* ptr = darr->first;
    for (i = 0; i < len;) {
        if (j < ptr->cap) {
            if (ptr->arr[j] != str[i]) {
                return false;
            }
            i++; j++;
        }
        else {
            ptr = ptr->next;
            j = 0;
        }
    }

    return true;
}

// note:
//    remember to release the memory of strings returned by GetStr function.
//    because the strings are generated by malloc function.
//
char* dynamicArrCharGetStr(DynamicArrChar* darr) {
    int64 i;
    int64 j = 0;
    // alloc an extra byte to add an '\0'. because when the string is using
    // outside the dynamic array, it will be used to do some operation like
    // strcmp or strcpy, and they all think the '\0' is the end of a string.
    //
    char* str = (char*)mem_alloc((sizeof(char)*darr->used) + 1);
    DynamicArrCharNode* node;
    for (node = darr->first; node != NULL; node = node->next) {
        for (i = 0; i < node->i; i++) {
            str[j] = node->arr[i];
            j++;
        }
    }
    str[darr->used] = '\0';
    return str;
}

// DynamicArrCharClear is used to clear the content in the dynamic char array.
// DynamicArrCharDestroy is used to destroy the whole array (including the memory
// occupied by the array).
//
void dynamicArrCharClear(DynamicArrChar* darr) {
    if (darr->first == NULL) {
        return;
    }
    DynamicArrCharNode* node = darr->first->next;
    DynamicArrCharNode* del;
    for (;;) {
        if (node == NULL) {
            darr->first->i    = 0;
            darr->first->next = NULL;
            darr->used        = 0;
            darr->total_cap   = darr->first->cap;
            darr->cur         = darr->first;
            return;
        }
        del  = node;
        node = node->next;
        mem_free(del->arr);
        mem_free(del);
    }
}

void dynamicArrCharDestroy(DynamicArrChar* darr) {
    DynamicArrCharNode* node = darr->first;
    DynamicArrCharNode* del;
    for (;;) {
        if (node == NULL) {
            darr->first = NULL;
            darr->cur   = NULL;
            return;
        }
        del  = node;
        node = node->next;
        mem_free(del->arr);
        mem_free(del);
    }
}
