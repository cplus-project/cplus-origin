/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The convert.h and convert.c implements some
 * operations about type and data transition.
 **/

#ifndef CPLUS_CONVERT_H
#define CPLUS_CONVERT_H

#include <string.h>
#include "dynamicarr.h"

extern char* conv_itoa             (int64 num);
extern int64 conv_binary_to_decimal(char* binary_num, int64 conv_len);

#endif
