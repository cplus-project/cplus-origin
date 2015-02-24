/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The test for scope.h and scope.c
 **/

#include "../scope.h"

int main() {
    error err = NULL;
    scope scp1; // the toplevel scope
    scope scp2;
    scope scp3; // the innermost scope

    scope_init(&scp1, NULL);
    scope_init(&scp2, &scp1);
    scope_init(&scp3, &scp2);

    id_info id1;
    id1.id_name     = "count";
    id1.id_len      = 5;
    id1.id_type     = ID_VAR;
    id1.id_datatype = "int32";
    id1.id_value    = "1";
    if (scope_insert_id(&scp1, id1) != NULL) {
        debug("error occur 1");
        return 0;
    }
    scope_debug(&scp1);

    debug("\r\n");

    id_info id2;
    id2.id_name     = "flag";
    id2.id_len      = 4;
    id2.id_type     = ID_VAR;
    id2.id_datatype = "bool";
    id2.id_value    = "true";
    if (scope_insert_id(&scp2, id2) != NULL) {
        debug("error occur 2");
        return 0;
    }
    scope_debug(&scp2);

    debug("\r\n");

    id_info id3;
    id3.id_name     = "PATH_MAX_LEN";
    id3.id_len      = 12;
    id3.id_type     = ID_CONST;
    id3.id_datatype = "int32";
    id3.id_value    = "255";
    if (scope_insert_id(&scp3, id3) != NULL) {
        debug("error occur 3");
        return 0;
    }
    scope_debug(&scp3);

    printf("\r\n\r\n--- The Update Test Below ---\r\n");
    id_info id_modify;
    id_modify = id3;
    id_modify.id_value = "1024";
    if (scope_update_id(&scp3, id_modify) != NULL) {
        debug("update error 1");
        return 0;
    }
    debug("update test 1 success...");
    id_modify = id1;
    id_modify.id_value = "9999";
    if (scope_update_id(&scp3, id_modify) != NULL) {
        debug("update error 2");
        return 0;
    }
    debug("update test 2 success...");
    id_modify.id_name = "not_in_table";
    id_modify.id_len = 12;
    err = scope_update_id(&scp3, id_modify);
    if (err != NULL) {
        debug("update test 3 success... this id is not in the table.");
    }
    else {
        debug("update error 3");
        return 0;
    }
    scope_debug(&scp1);
    scope_debug(&scp2);
    scope_debug(&scp3);

    printf("\r\n\r\n--- The Search Test Below ---\r\n");
    id_info search;
    search.id_name = id3.id_name;
    search.id_len  = id3.id_len;
    if (scope_search_id(&scp3, &search) != NULL) {
        debug("search error 1");
        return 0;
    }
    printf("search 1 success... the value of id is : %s\r\n", search.id_value);
    search.id_name = id1.id_name;
    search.id_len  = id1.id_len;
    if (scope_search_id(&scp3, &search) != NULL) {
        debug("search error 2");
        return 0;
    }
    printf("search 2 success... the value of id is : %s\r\n", search.id_value);
    search.id_name = "not_in_table";
    search.id_len  = 12;
    err = scope_search_id(&scp3, &search);
    if (err != NULL) {
        debug("search 3 success... the id is not in the table.");
    }
    else {
        debug("search error 3");
        return 0;
    }

    scope_destroy(&scp3);
    scope_destroy(&scp2);
    scope_destroy(&scp1);

    debug("run over~");
    return 0;
}
