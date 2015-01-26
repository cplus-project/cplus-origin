#include "common.h"

error new_error(char* errmsg) {
    if (errmsg == NULL) {
        return "";
    } else {
        return errmsg;
    }
}
