/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "common.h"

error new_error(char* errmsg) {
    if (errmsg == NULL) {
        return "";
    }
    else {
        return errmsg;
    }
}
