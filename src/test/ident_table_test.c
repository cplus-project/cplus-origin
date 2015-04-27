/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The test for ident_table in ident.h/ident.c
 **/

#include "../ident.h"

int main() {
    ident_table itab;
    ident_table_init(&itab);

    ident* ids[20];
    ids[0] = make_id_const("OS", ACCESS_OT, "string", "linux");
    ids[1] = make_id_const("MAX_PATH_LEN", ACCESS_OT, "int", "255");
    ids[2] = make_id_var("path", ACCESS_IN, "string", "/home/John/Music/forever.mp3");
    ids[3] = make_id_var("mode", ACCESS_IN, "int8", "0");

    ids[4] = make_id_type("Music", ACCESS_OT);
    ident* id4_members[5];
    {
        id4_members[0] = make_id_var("music_name", ACCESS_IN, "string", "");
        id4_members[1] = make_id_var("music_date", ACCESS_IN, "Date", );
    }

    ident_table_destroy(&itab);
    return 0;
}
