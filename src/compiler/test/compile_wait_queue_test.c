/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The test for CompileWaitQueue in file imptcache.h
 * and imptcache.c
 **/

#include "../imptcache.h"

static void compileWaitQueueDebug(CompileWaitQueue* waitqueue) {
    printf("the cur pointer is: %s\r\n", waitqueue->cur->file_info.file_name);
    printf("all nodes in the wait queue:\r\n");
    CompileWaitQueueNode* ptr;
    for (ptr = waitqueue->head->next; ptr != NULL; ptr = ptr->next) {
        printf("%s ", ptr->file_info.file_name);
    }
    printf("\r\n");
}

int main() {
    CompileWaitQueue waitqueue;
    compileWaitQueueInit(&waitqueue);
    
    printf("before insert the main.cplus:");
    compileWaitQueueIsEmpty(&waitqueue) == true ? debug("the wait queue is empty.") : debug("the wait queue is not empty.");
    compileWaitQueueEnqueue(&waitqueue, "main.cplus");
    printf("after insert the main.cplis:");
    compileWaitQueueIsEmpty(&waitqueue) == true ? debug("the wait queue is empty.") : debug("the wait queue is not empty.");
        
    compileWaitQueueGetFile(&waitqueue);
    printf("\r\nhave called GetFile() function here already, now insert nodes with the order: h1.cplus h2.cplus h3.cplus:\r\n");
    compileWaitQueueEnqueue(&waitqueue, "h1.cplus");
    compileWaitQueueEnqueue(&waitqueue, "h2.cplus");
    compileWaitQueueEnqueue(&waitqueue, "h3.cplus");
    compileWaitQueueDebug(&waitqueue);
    
    printf("\r\nnow try to dequeue the nodes from head:\r\n");
    printf("\r\ndequeue 1:\r\n");
    compileWaitQueueDequeue(&waitqueue);
    compileWaitQueueGetFile(&waitqueue);
    compileWaitQueueDebug(&waitqueue);
    printf("\r\ndequeue 2:\r\n");
    compileWaitQueueDequeue(&waitqueue);
    compileWaitQueueGetFile(&waitqueue);
    compileWaitQueueDebug(&waitqueue);
    
    printf("\r\ncontinue to do the operations:\r\n");
    printf("(1) insert h4.cplus h5.cplus\r\n");
    printf("(2) call GetFile() to reset 'cur' position\r\n");
    printf("(3) insert h6.cplus\r\n");
    compileWaitQueueEnqueue(&waitqueue, "h4.cplus");
    compileWaitQueueEnqueue(&waitqueue, "h5.cplus");
    compileWaitQueueGetFile(&waitqueue);
    compileWaitQueueEnqueue(&waitqueue, "h6.cplus");
    compileWaitQueueDebug(&waitqueue);
    
    compileWaitQueueDestroy(&waitqueue);
    debug("\r\n～test over～");
    return 0;
}
