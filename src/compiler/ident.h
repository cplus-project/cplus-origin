/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     This this implements a red-black tree to storage the
 * information of nameable objects.
 **/

#ifndef CPLUS_IDENT_H
#define CPLUS_IDENT_H

#include "common.h"

typedef struct Ident           Ident;
typedef struct IdentUnresolved IdentUnresolved;
typedef struct IdentDatatype   IdentDatatype;
typedef struct IdentVariable   IdentVariable;
typedef struct IdentArray      IdentArray;
typedef struct IdentFunction   IdentFunction;
typedef struct IdentExpander   IdentExpander;
typedef struct IdentModule     IdentModule;
typedef struct IdentTableNode  IdentTableNode;
typedef struct IdentTable      IdentTable;

#define ID_TYPE_UNRESOLVED 0
#define ID_TYPE_DATATYPE   1
#define ID_TYPE_VARIABLE   2
#define ID_TYPE_ARRAY      3
#define ID_TYPE_FUNCTION   4
#define ID_TYPE_EXPANDER   5
#define ID_TYPE_MODULE     6

// Ident can represent any identifier in Cplus. the member id will be used based on
// the value of the id_type.
//
// for example, if the id_type is ID_TYPE_VARIABLE, then the actual effective pointer
// of id is id.id_variable.
//
struct Ident {
    char* id_name;
    int8  id_type;
    union {
        IdentUnresolved* id_unresolved;
        IdentDatatype*   id_datatype;
        IdentVariable*   id_variable;
        IdentArray*      id_array;
        IdentFunction*   id_function;
        IdentExpander*   id_expander;
        IdentModule*     id_module;
    }id;
};

// IdentUnresolved represents an identifier can not resolved immediately but can be
// resolved later.
//
struct IdentUnresolved {  
    char* mod_name;
    char* id_name;
};

#define DATATYPE_UNASSIGNED        NULL
#define DATATYPE_BASETYPE_BYTE     -1
#define DATATYPE_BASETYPE_INT8     -2
#define DATATYPE_BASETYPE_INT16    -3
#define DATATYPE_BASETYPE_INT32    -4
#define DATATYPE_BASETYPE_INT64    -5
#define DATATYPE_BASETYPE_INT128   -6
#define DATATYPE_BASETYPE_UINT8    -7
#define DATATYPE_BASETYPE_UINT16   -8
#define DATATYPE_BASETYPE_UINT32   -9
#define DATATYPE_BASETYPE_UINT64   -10
#define DATATYPE_BASETYPE_UINT128  -11
#define DATATYPE_BASETYPE_FLOAT32  -12
#define DATATYPE_BASETYPE_FLOAT64  -13
#define DATATYPE_BASETYPE_CHAR     -14
#define DATATYPE_BASETYPE_STRING   -15

struct IdentDatatype {
    IdentTable* id_table;
};

struct IdentVariable {
    IdentDatatype* datatype;
};

struct IdentArray {
    int64          length;
    IdentDatatype* datatype;
};

struct IdentFunction {
    
};

struct IdentExpander {
    
};

struct IdentModule {
    IdentTable* id_table;
};

#define NODE_COLOR_RED   0x00
#define NODE_COLOR_BLACK 0x01

#define NODE_CMP_LT      0x00
#define NODE_CMP_EQ      0x01
#define NODE_CMP_GT      0x02

struct IdentTableNode {
    Ident*          id;
    int8            color;
    IdentTableNode* parent;
    IdentTableNode* lchild;
    IdentTableNode* rchild;
};

// the IdentTable is used to storage a set of information about nameable objects in
// Cplus language.
//
struct IdentTable {
    IdentTableNode* root;
};

extern void   identTableInit   (IdentTable* id_table);
extern error  identTableAdd    (IdentTable* id_table, Ident* id);
extern Ident* identTableSearch (IdentTable* id_table, char*  id_name);
extern void   identTableDestroy(IdentTable* id_table);

#endif
