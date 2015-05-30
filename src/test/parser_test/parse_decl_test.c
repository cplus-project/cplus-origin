/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The test abount  parsing declaration statement.
 **/

#include "../../syntax.h"

int main() {
    syntax_analyzer syx;
    syntax_analyzer_init(&syx, "./test/parser_test/parse_decl_test.cplus");
    error err = syntax_analyzer_work(&syx);
    if (err != NULL) {
        if (ERROR_CODE(err) == LEX_ERROR_EOF) {
            debug("parsing complete");
        }
        else {
            debug(err);
        }
    }
    return 0;
}