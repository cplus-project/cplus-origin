/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The test for utf.h and utf.c
 **/

#include "../utf.h"

int main() {
    utf8_code code = 50;
    char* rune = utf8_code_to_char(code);
    printf("%s\r\n", rune);
    return 0;
}
