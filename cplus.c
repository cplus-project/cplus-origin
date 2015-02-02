#include <stdio.h>
#include <string.h>
#include "common.h"
#include "dynamicarr.h"
#include "lex.h"
#include "convert.h"

int main() {
    int64 i;
    for (i=-100; i<=123; i++) {
        char* str = conv_itoa(i);
        printf("%s\r\n", str);
    }
    return 0;
}
