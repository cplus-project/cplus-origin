/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "scope.h"

void scope_init(scope* scp, scope* outer) {
    scp->outer = outer;
    idtable_init(&scp->idt);
}

error scope_insert_id(scope* scp, id_info id) {
    return idtable_insert(&scp->idt, id);
}

error scope_update_id(scope* scp, id_info  new_info) {
    return idtable_update(&scp->idt, new_info);
}

error scope_search_id(scope* scp, id_info* ret) {
    return idtable_search(&scp->idt, ret);
}

void scope_destroy(scope* scp) {
    scp->outer = NULL;
    idtable_destroy(&scp->idt);
}
