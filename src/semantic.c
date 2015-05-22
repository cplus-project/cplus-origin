/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "semantic.h"

void semantic_analyzer_init(semantic_analyzer* smt) {
    smt->cur_scp = NULL;
}

void semantic_analyzer_scope_open(semantic_analyzer* smt) {
    scope* scp = (scope*)mem_alloc(sizeof(scope));
    scope_init(scp, smt->cur_scp);
}

void semantic_analyzer_scope_close(semantic_analyzer* smt) {
    scope* temp = smt->cur_scp;
    smt->cur_scp = smt->cur_scp->outer;
    scope_destroy(temp);
}
