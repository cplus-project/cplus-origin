#include <string.h>
#include "common.h"
#include "dynamicarr.h"
#include "lex.h"

int main() {
    dynamicarr_char darr;
    error err = dynamicarr_char_init(&darr, 1);
    if (err != NULL) {
        printf("%s\r\n", err);
        return 0;
    }
    char* msg = "aaaaa";
    dynamicarr_char_append(&darr, msg, strlen(msg));
    dynamicarr_char_debug(&darr);
    dynamicarr_char_destroy(&darr);
    return 0;
}

