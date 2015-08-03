/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The test for path.h and path.c.
 **/

#include "../path.h"
#include <dirent.h>

int main() {
    char* last = NULL;
    char* path = "/home/user/test.c";
    printf("[unix-like]\r\n");
    for (;;) {
        printf("the path now is: %s", path);
        printf("\tthe last level of the path is: %s\r\n", path_last(path, strlen(path)));
        path = path_prev(path, strlen(path));
        if (path == NULL) {
            break;
        }
    }
    
    path = "C:/Windows/System32";
    printf("\r\n[windows]\r\n");
    for (;;) {
        printf("the path now is: %s", path);
        printf("\tthe last level of the path is: %s\r\n", path_last(path, strlen(path)));
        path = path_prev(path, strlen(path));
        if (path == NULL) {
            break;
        }
    }
    
    if (path_isdir("/home") == NULL) printf("\r\n/home is directory\r\n");
    if (path_isreg("/etc/network/interfaces") == NULL) printf("/etc/network/interfaces is regular file\r\n");
    
    debug("\r\ntest over\r\n");
    return 0;
}
