/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The test for close_counter.h and close_counter.c
 **/

#include "../close_counter.h"

int main() {
    error err = NULL;
    close_counter clsctr;
    close_counter_init(&clsctr);

    if (close_counter_is_clear(&clsctr) == true) {
        printf("here should be empty: ok\r\n");
    }
    else {
        printf("here should be empty: false\r\n");
    }

    close_counter_increase(&clsctr, '(');
    close_counter_increase(&clsctr, '[');
    close_counter_increase(&clsctr, '{');
    close_counter_increase(&clsctr, '{');
    close_counter_increase(&clsctr, '[');
    close_counter_increase(&clsctr, '(');

    if (close_counter_is_clear(&clsctr) == false) {
        printf("here should not be empty: ok\r\n");
    }
    else {
        printf("here should not be empty: false\r\n");
    }

    printf("\r\n");
    close_counter_debug(&clsctr);

    err = close_counter_decrease(&clsctr, '}');
    if (err != NULL) {
        printf("\r\n\r\nhere should report an error: ok\r\n");
        printf("the error message is: %s\r\n", err);
    }

    err = close_counter_decrease(&clsctr, ')');
    if (err == NULL) {
        printf("\r\nhere should not report an error: ok\r\n");
    }

    close_counter_decrease(&clsctr, ']');
    close_counter_decrease(&clsctr, '}');
    close_counter_decrease(&clsctr, '}');

    if (close_counter_is_clear(&clsctr) == false) {
        printf("here should not be empty: ok\r\n");
    }
    else {
        printf("here should not be empty: false\r\n");
    }

    close_counter_decrease(&clsctr, ']');
    close_counter_decrease(&clsctr, ')');

    if (close_counter_is_clear(&clsctr) == true) {
        printf("here should be empty: ok\r\n");
    }
    else {
        printf("here should be empty: false\r\n");
    }

    err = close_counter_decrease(&clsctr, ')');
    if (err != NULL) {
        printf("\r\nhere should report that the counter is empty: %s\r\n\r\n", err);
    }

    close_counter_destroy(&clsctr);
    debug("run over~");
    return 0;
}
