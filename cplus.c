#include <string.h>
#include "common.h"
#include "lex.h"
#include "dynamicarr.h"

int main() {
    int i = 0;
    char* str = "hello world!";
    int64 len = strlen(str);
    dynamicarr_char arr;
    dynamicarr_char_init(&arr, 10);
    dynamicarr_char_append(&arr, str, len);
    char* ret = dynamicarr_char_getstr(&arr);
    dynamicarr_char_destroy(&arr);
    printf("%s\n", ret);
    return 0;
}
