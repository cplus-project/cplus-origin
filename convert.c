/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "convert.h"

char* conv_itoa(int64 num) {
    bool negative = false;
    if (num < 0) {
        num = -num;
        negative = true;
    }
    dynamicarr_char darr;
    dynamicarr_char_init(&darr, 64);
    for (;;) {
        dynamicarr_char_appendc(&darr, num%10 + 48);
        if (num/10 == 0) {
            break;
        }
        num /= 10;
    }
    if (negative == true) {
        dynamicarr_char_appendc(&darr, '-');
    }
    char  ch;
    char* numstr      = dynamicarr_char_getstr(&darr);
    int   reverse_len = darr.used;
    int   reverse_end = reverse_len >> 1;
    int   i;
    dynamicarr_char_destroy(&darr);
    for (i = 0; i < reverse_end; i++) {
        ch = numstr[i];
        numstr[i] = numstr[reverse_len-i-1];
        numstr[reverse_len-i-1] = ch;
    }
    return numstr;
}


// transfor binary format to the decimal format.
// for example: 1011 = 1<<3 + 1<<1 + 1<<0
//              0110 = 1<<2 + 1<<1
int64 conv_binary_to_decimal(char* binary_num, uint64 conv_len) {
    int   i;
    int   digit  = conv_len - 1;
    int64 decval = 0;
    for (i = 0; i < conv_len; i++) {
        if (binary_num[i] == '1') {
            decval += 1<<digit;
        }
        digit--;
    }
    if (binary_num[0] == '-') {
        decval = -decval;
    }
    return decval;
}
