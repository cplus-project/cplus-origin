// the test for scope.h and scope.c

#include "../common.h"
#include "../scope.h"

int main() {
    error err = NULL;
    scope scope1; // the root scope
    scope scope2;
    scope scope3;

    scope_init(&scope1, NULL);
    scope_init(&scope2, &scope1);
    scope_init(&scope3, &scope2);

    id_info id_count;
    id_count.id_name     = "count";
    id_count.id_len      = 5;
    id_count.id_type     = ID_VAR;
    id_count.id_datatype = "int32";
    id_count.id_value    = 1;
    if (scope_insert_id(&scope1, id_count) != NULL) {
        debug("some error occur 1:");
        debug(err);
        return 0;
    }

    id_info id_flag;
    id_flag.id_name     = "hide";
    id_flag.id_len      = 4;
    id_flag.id_type     = ID_VAR;
    id_flag.id_datatype = "bool";
    id_flag.id_value    = "false";
    if (scope_insert_id(&scope2, id_flag) != NULL) {
        debug("some error occur 2:");
        debug(err);
        return 0;
    }

    id_info id_message;
    id_message.id_name     = "str";
    id_message.id_len      = 3;
    id_message.id_type     = ID_VAR;
    id_message.id_datatype = "string";
    id_message.id_value    = "Hello! Jack.";
    if (scope_insert_id(&scope3, id_message) != NULL) {
        debug("some error occur 3:");
        debug(err);
        return 0;
    }

    scope_debug(&scope1);
    scope_debug(&scope2);
    scope_debug(&scope3);

    scope_destroy(&scope3);
    scope_destroy(&scope2);
    scope_destroy(&scope1);

    return 0;
}
