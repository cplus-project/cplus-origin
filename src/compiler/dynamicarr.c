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

    // the darr->first is the first buffer of the dynamic char
    // array. this buffer's capacity is the same as the parameter
    // passed as 'capacity'. if requirement of size is bigger
    // than the darr->total_cap, then a new buffer will created
    // and be linked with the darr->next pointer.
    darr->first       = (DynamicArrCharNode*)mem_alloc(sizeof(DynamicArrCharNode));
    darr->first->cap  = capacity;
    darr->first->i    = 0;
    darr->first->arr  = (char*)mem_alloc(sizeof(char) * capacity);
    darr->first->next = NULL;

    darr->cur = darr->first;
    return NULL;
}

void dynamicArrCharAppend(DynamicArrChar* darr, char* str, int64 len) {
    int64 j = 0;
    int64 need = darr->used + len;
    if (need <= darr->total_cap) {
        for (j = 0; j < len; j++) {
            darr->cur->arr[darr->cur->i] = str[j];
            darr->cur->i++;
            darr->used++;
        }
    }
    else {
        // if the total size of now used space of dynamic char array and
        // the append string is more than darr->total_size. then extend
        // the total capacity of the dynamic array. we create a new buffer
        // with twice size of current total capacity every time until it
        // can contain the requirement size of the string.
        DynamicArrCharNode* _cur = darr->cur;
        for(;;) {
            if (darr->total_cap < need) {
                _cur->next = mem_alloc(sizeof(DynamicArrCharNode));
                _cur->next->cap  = darr->total_cap;
                _cur->next->i    = 0;
                _cur->next->arr  = (char*)mem_alloc(sizeof(char) * darr->total_cap);
                _cur->next->next = NULL;
                _cur = _cur->next;
                darr->total_cap *= 2;
            }
            else {
                for (j = 0; j < len;) {
                    if (darr->cur->i < darr->cur->cap) {
                        darr->cur->arr[darr->cur->i] = str[j];
                        darr->cur->i++;
                        darr->used++;
                        j++;
                    }
                    else {
                        darr->cur = darr->cur->next;
                    }
                }
                return;
            }
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

// return true if the given string is equal to the content
// in the dynamic char array.
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

char* dynamicArrCharGetStr(DynamicArrChar* darr) {
    int64 i;
    int64 j   = 0;
    int64 len = darr->used;
    // alloc an extra byte to add an '\0'. because when the string is using
    // outside the dynamic array, it will be used to do some operation like
    // strcmp or strcpy, and they all think the '\0' is the end of a string.
    char* str = (char*)mem_alloc(sizeof(char)*len + 1);
    DynamicArrCharNode* ptr = darr->first;
    for (i = 0; i < len;) {
        if (j < ptr->cap) {
            str[i] = ptr->arr[j];
            j++; i++;
        }
        else {
            ptr = ptr->next;
            j = 0;
        }
    }
    str[len] = '\0';
    return str;
}

// DynamicArrCharClear is used to clear the content in the dynamic
// char array. DynamicArrCharDestroy is used to destroy the whole
// array (including the memory occupied by the array).
void dynamicArrCharClear(DynamicArrChar* darr) {
    if (darr->first != NULL) {
        darr->first->i  = 0;
        darr->used      = 0;
        darr->total_cap = darr->first->cap;
    }
    if (darr->first->next != NULL) {
        DynamicArrCharNode* ptr  = darr->first->next;
        DynamicArrCharNode* temp = NULL;
        for (;;) {
            temp = (DynamicArrCharNode*)ptr->next;
            if (ptr != NULL) {
                mem_free(ptr);
            }
            if (temp == NULL) {
                break;
            }
            else {
                ptr = temp;
            }
        }
        darr->first->next = NULL;
    }
    darr->cur = darr->first;
}

void dynamicArrCharDestroy(DynamicArrChar* darr) {
    if (darr->first != NULL) {
        DynamicArrCharNode* ptr  = darr->first;
        DynamicArrCharNode* temp = NULL;
        for (;;) {
            temp = (DynamicArrCharNode*)ptr->next;
            if (ptr != NULL) {
                mem_free(ptr);
            }
            if (temp == NULL) {
                break;
            }
            else {
                ptr = temp;
            }
        }
        darr->first = NULL;
        darr->cur   = NULL;
    }
}
