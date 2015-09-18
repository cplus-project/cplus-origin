/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The test for dynamicarr.h and dynamicarr.c
 **/

#include "../dynamicarr.h"

static void dynamicArrCharDebug(DynamicArrChar* darr) {
    if (darr->first == NULL) {
        printf("the dynamic array is destroy\r\n.");
        return;
    }
    DynamicArrCharNode* node;
    int i;
    int node_count = 1;
    char* content;

    printf("[NODES INFO] total capacity: %lld\r\n", darr->total_cap);
    for (node = darr->first; node != NULL; node = node->next) {
        printf("Node #%02d: ", node_count++);
        for (i = 0; i < node->i; i++) {
            printf("%c", node->arr[i]);
        }
        printf("\r\n");
    }
    content = dynamicArrCharGetStr(darr);
    printf("content: %s\r\n", content);
    printf("content length: %d\r\n", (int)strlen(content));
    mem_free(content);
    printf("\r\n");
}

int main() {
    DynamicArrChar darr;
    dynamicArrCharInit(&darr, 10);

    char* msg1 = "hello";
    dynamicArrCharAppend(&darr, msg1, strlen(msg1));
    dynamicArrCharDebug (&darr);

    dynamicArrCharAppendc(&darr, ',');
    dynamicArrCharDebug  (&darr);

    char* msg2 = "world";
    dynamicArrCharAppend(&darr, msg2, strlen(msg2));
    dynamicArrCharDebug (&darr);

    char* msg3 = ".I love you!";
    dynamicArrCharAppend(&darr, msg3, strlen(msg3));
    dynamicArrCharDebug (&darr);

    char* msg4 = "0123456789"
    "0123456789"
    "0123456789"
    "0123456789"
    "0123456789"
    "0123456789"
    "0123456789"
    "0123456789"
    "0123456789"
    "0123456789";
    dynamicArrCharAppend(&darr, msg4, strlen(msg4));
    dynamicArrCharDebug (&darr);

    printf("clean the dynamic array...\r\n");
    dynamicArrCharClear(&darr);
    dynamicArrCharDebug(&darr);

    printf("try to append content by another dynamic array...\r\n");
    DynamicArrChar _darr;
    dynamicArrCharInit      (&_darr, 5);
    dynamicArrCharAppend    (&_darr, msg4, strlen(msg4));
    dynamicArrCharAppendDarr(&darr, &_darr);
    dynamicArrCharDebug     (&darr);

    dynamicArrCharDestroy(&darr);
    dynamicArrCharDebug  (&darr);
    debug("run over");
    return 0;
}
