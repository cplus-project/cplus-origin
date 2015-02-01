/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include <string.h>
#include "common.h"
#include "dynamicarr.h"

error new_error(char* errmsg) {
    if (errmsg == NULL) {
        return "";
    }
    else {
        return errmsg;
    }
}

char* cplus_itoa(int64 num) {
    bool negative = false;
    if (num < 0) {
        num = -num;
        negative = true;
    }
    dynamicarr_char darr;
    dynamicarr_char_init(&darr, 64);
    for (;;) {
        switch (num%10) {
        case 0:
            dynamicarr_char_appendc(&darr, '0');
            break;
        case 1:
            dynamicarr_char_appendc(&darr, '1');
            break;
        case 2:
            dynamicarr_char_appendc(&darr, '2');
            break;
        case 3:
            dynamicarr_char_appendc(&darr, '3');
            break;
        case 4:
            dynamicarr_char_appendc(&darr, '4');
            break;
        case 5:
            dynamicarr_char_appendc(&darr, '5');
            break;
        case 6:
            dynamicarr_char_appendc(&darr, '6');
            break;
        case 7:
            dynamicarr_char_appendc(&darr, '7');
            break;
        case 8:
            dynamicarr_char_appendc(&darr, '8');
            break;
        case 9:
            dynamicarr_char_appendc(&darr, '9');
            break;
        }
        if (num/10 == 0) {
            break;
        }
        num /= 10;
    }
    if (negative == true) {
        dynamicarr_char_appendc(&darr, '-');
    }
    char* numstr = dynamicarr_char_getstr(&darr);
    dynamicarr_char_destroy(&darr);
    int  reverse_len = strlen(numstr);
    int  reverse_end = reverse_len >> 1;
    int  i;
    char ch;
    for (i = 0; i < reverse_end; i++) {
        ch = numstr[i];
        numstr[i] = numstr[reverse_len-i-1];
        numstr[reverse_len-i-1] = ch;
    }
    return numstr;
}
