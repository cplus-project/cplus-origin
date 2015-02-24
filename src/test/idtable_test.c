/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The test for idtable.h and idtable.c
 **/

#include "../idtable.h"

int main() {
    int i;
    id_info update;
    id_info search;
    idtable idt;
    idtable_init(&idt);

    id_info ids[15];
    ids[0].id_name     = "count";
    ids[0].id_len      = 5;
    ids[0].id_type     = ID_VAR;
    ids[0].id_datatype = "uint32";
    ids[0].id_value    = "5";

    ids[1].id_name     = "flag";
    ids[1].id_len      = 4;
    ids[1].id_type     = ID_VAR;
    ids[1].id_datatype = "bool";
    ids[1].id_value    = "true";

    ids[2].id_name     = "prefix";
    ids[2].id_len      = 6;
    ids[2].id_type     = ID_VAR;
    ids[2].id_datatype = "string";
    ids[2].id_value    = "err_";

    ids[3].id_name     = "i";
    ids[3].id_len      = 1;
    ids[3].id_type     = ID_VAR;
    ids[3].id_datatype = "int32";
    ids[3].id_value    = "0";

    ids[4].id_name     = "file";
    ids[4].id_len      = 4;
    ids[4].id_type     = ID_VAR;
    ids[4].id_datatype = "File";
    ids[4].id_value    = "nil";

    ids[5].id_name     = "j";
    ids[5].id_len      = 1;
    ids[5].id_type     = ID_VAR;
    ids[5].id_datatype = "int32";
    ids[5].id_value    = "100";

    ids[6].id_name     = "MAX_BUFF_SIZE";
    ids[6].id_len      = 13;
    ids[6].id_type     = ID_CONST;
    ids[6].id_datatype = "int32";
    ids[6].id_value    = "0x01";

    ids[7].id_name     = "req";
    ids[7].id_len      = 3;
    ids[7].id_type     = ID_VAR;
    ids[7].id_datatype = "Request";
    ids[7].id_value    = "nil";

    ids[8].id_name     = "url";
    ids[8].id_len      = 3;
    ids[8].id_type     = ID_VAR;
    ids[8].id_datatype = "URL";
    ids[8].id_value    = "http://www.host.com/";

    ids[9].id_name     = "buff";
    ids[9].id_len      = 4;
    ids[9].id_type     = ID_VAR;
    ids[9].id_datatype = "Buffer";
    ids[9].id_value    = "[25, 13, 13, 122]";

    ids[10].id_name     = "buffer";
    ids[10].id_len      = 6;
    ids[10].id_type     = ID_VAR;
    ids[10].id_datatype = "char[]";
    ids[10].id_value    = "hello world";

    ids[11].id_name     = "max";
    ids[11].id_len      = 3;
    ids[11].id_type     = ID_VAR;
    ids[11].id_datatype = "int64";
    ids[11].id_value    = "74661231";

    ids[12].id_name     = "signal";
    ids[12].id_len      = 6;
    ids[12].id_type     = ID_VAR;
    ids[12].id_datatype = "int32";
    ids[12].id_value    = "121";

    ids[13].id_name     = "ptr";
    ids[13].id_len      = 3;
    ids[13].id_type     = ID_VAR;
    ids[13].id_datatype = "ListNode*";
    ids[13].id_value    = "nil";

    ids[14].id_name     = "btn";
    ids[14].id_len      = 3;
    ids[14].id_type     = ID_VAR;
    ids[14].id_datatype = "Botton";
    ids[14].id_value    = "nil";

    for (i = 0; i < 15; i++) {
        if (idtable_insert(&idt, ids[i]) != NULL) {
            printf("node %02d: ", i);
            debug("error occur");
        }
    }
    idtable_debug(&idt);

    printf("\r\n\r\n--- The Update Test Below ---\r\n");
    for (i = 0; i < 15; i++) {
        update = ids[i];
        if (update.id_value == "nil") {
            update.id_value = "<nil>";
            if (idtable_update(&idt, update) != NULL) {
                printf("the node %d\r\n with the id name %s update failed...\r\n", i+1, update.id_name);
            }
        }
    }
    idtable_debug(&idt);

    printf("\r\n\r\n--- The Search Test Below ---\r\n");
    for (i = 0; i < 15; i++) {
        search.id_name = ids[i].id_name;
        search.id_len  = ids[i].id_len;
        printf("search %s: ", search.id_name);
        if (idtable_search(&idt, &search) == NULL) {
            debug("ok");
        }
        else {
            debug("no");
        }
    }

    idtable_destroy(&idt);

    debug("\r\nrun over~");
    return 0;
}
