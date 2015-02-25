/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The test for utf.h and utf.c
 **/

#include "../utf.h"

int main() {
    unicode_code unicode = 0x4E25;
    utf8_code utf8 = utf8_encode(unicode);
    printf("the UTF-8 code of Unicode 0x%X is: 0x%X\r\n", unicode, utf8);

    char* str = "hello world?你好，世界！我非常喜欢编程";
    int i, j;
    int bytes;
    utf8_char rune;
    printf("测试样例: %s\r\n", str);
    printf("测试结果: ");
    for (i = 0; i < strlen(str);) {
        bytes = utf8_calcu_bytes(str[i]);
        switch (bytes) {
        case 1:
            rune.bytes = 1;
            rune.utf8_bytes.utf8_bytes_1 = str[i]; i++;
            break;
        case 2:
            rune.bytes = 2;
            rune.utf8_bytes.utf8_bytes_2[0] = str[i]; i++;
            rune.utf8_bytes.utf8_bytes_2[1] = str[i]; i++;
            break;
        case 3:
            rune.bytes = 3;
            rune.utf8_bytes.utf8_bytes_3[0] = str[i]; i++;
            rune.utf8_bytes.utf8_bytes_3[1] = str[i]; i++;
            rune.utf8_bytes.utf8_bytes_3[2] = str[i]; i++;
            break;
        case 4:
            rune.bytes = 4;
            rune.utf8_bytes.utf8_bytes_4[0] = str[i]; i++;
            rune.utf8_bytes.utf8_bytes_4[1] = str[i]; i++;
            rune.utf8_bytes.utf8_bytes_4[2] = str[i]; i++;
            rune.utf8_bytes.utf8_bytes_4[3] = str[i]; i++;
            break;
        default:
            debug("error occur 1");
            return 0;
        }
        utf8_print(rune);
    }
    debug("\r\n");

    debug("run over~");
    return 0;
}
