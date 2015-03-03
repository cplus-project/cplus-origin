/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The test for the decl_list defined in src/id.h
 * and src/id.c
 **/

#include "../id.h"

int main() {
    int i;
    declare decls[4];

    decls[0].decl_type   = "int32";
    decls[0].decl_name   = "i";
    decls[0].decl_assign = "0";

    decls[1].decl_type   = "int32";
    decls[1].decl_name   = "count";
    decls[1].decl_assign = "1";

    decls[2].decl_type   = "string";
    decls[2].decl_name   = "filename";
    decls[2].decl_assign = "/usr/bin/ls";

    decls[3].decl_type   = "char";
    decls[3].decl_name   = "ch";
    decls[3].decl_assign = "'a'";

    decl_list declist;
    decl_list_init(&declist);
    for (i = 0; i < 4; i++) {
        decl_list_add(&declist, decls[i]);
    }
    decl_list_debug(&declist);
    decl_list_destroy(&declist);

    debug("\r\nrun over~");
    return 0;
}
