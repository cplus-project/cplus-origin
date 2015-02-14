/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

 #include "scope.h"

void scope_init(scope* scp, scope* outer) {
    scp->outer = outer;
}

error scope_insert_id(scope* scp, id_info  id) {}

error scope_update_id(scope* scp, id_info  new_info) {}

error scope_search_id(scope* scp, id_info* ret) {}