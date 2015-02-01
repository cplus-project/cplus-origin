#include <stdio.h>
#include <string.h>
#include "common.h"
#include "dynamicarr.h"
#include "lex.h"

int main() {
    int64 i;
    for (i=-100; i<=123; i++) {
        char* str = cplus_itoa(i);
        printf("%s\r\n", str);
    }
    return 0;
}
