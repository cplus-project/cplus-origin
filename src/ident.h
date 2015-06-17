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

#define ID_UNKNOWN         -1
#define ID_PLACEHOLDER   0x00
#define ID_CONST         0x01
#define ID_VAR           0x02
#define ID_ARRAY         0x03
#define ID_TYPE          0x04
#define ID_FUNCTION      0x05
#define ID_EXPANDER      0x06
#define ID_INCLUDE       0x07
#define ID_MODULE        0x08

#define ACCESS_NULL      0x00
#define ACCESS_IN        0x01
#define ACCESS_OT        0x02

#define NODE_COLOR_RED   0x00
#define NODE_COLOR_BLACK 0x01

#define NODE_CMP_LT      0x00
#define NODE_CMP_EQ      0x01
#define NODE_CMP_GT      0x02

typedef struct Ident        Ident;
typedef struct IdentConst   IdentConst;
typedef struct IdentVar     IdentVar;
typedef struct IdentArray   IdentArray;
typedef struct IdentType    IdentType;
typedef struct IdentFunc    IdentFunc;
typedef struct IdentExpn    IdentExpn;
typedef struct IdentInclude IdentInclude;
typedef struct IdentModule  IdentModule;
typedef struct IdentTable   IdentTable;

// represent a constant's information. a constant can not be
// assigned more than once.
struct IdentConst {
    IdentType* data_type_info;
};

// represent a variable's information. a variable is an address's
// name, it can be used to modify the address's content frequently.
struct IdentVar {
    IdentType* data_type_info;
};

// represent an array's information.
struct IdentArray {
    IdentType* data_type_info;
};

// represent the member's information of a type. it is only used
// when the type's primitive is false(means the type is a compound
// type).
typedef struct Member {
    char* member_name;
    int8  member_access;
    int8  member_type;
    union {
        IdentConst* constant;
        IdentVar*   variable;
        IdentArray* array;
        IdentType*  type;
        IdentFunc*  function;
        IdentExpn*  expander;
    }member;

    struct Member* next;
}Member;

// represent a type's information.
struct IdentType {
    bool primitive;
    union {
        int16   prim_type_token_code;
        Member* members;
    }type;
};

// represent a parameter passed into or returned out by a function
// or a expander.
typedef struct Parameter {
    IdentType* param_data_type_info;
    char*      param_name;

    struct Parameter* next;
}Parameter;

// represent a function's information.
struct IdentFunc {
    Parameter* params_passin;
    Parameter* params_retout;
    // TODO: design a way to represent the error_tag list
};

// represent a expander's information. the expander is often called
// 'micro' in other languages.
struct IdentExpn {
    Parameter* params_passin;
    // TODO: design a way to represent the expander's body
};

// represent an included file's information about identified objects
// within it.
struct IdentInclude {
    IdentTable* id_table;
};

// represent a module's information about identified objects within it.
struct IdentModule {
    IdentTable* id_table;
};

// an Ident represents an identified object.
//
// the example about using an Ident object:
//    IdentTable id_table;
//    ...
//    Ident* id = identTableSearch(&id_table, "foo");
//    switch (id->id_type) {
//    case ID_CONST: // do some process to the 'id->id_info.id_const'
//    case ID_VAR:   // do some process to the 'id->id_info.id_var'
//    case ID_ARRAY: // do some process to the 'id->id_info.id_array'
//    ...
//    default: ...
//    }
//    ...
struct Ident {
    char* id_name;
    int8  access;
    int8  id_type;
    union {
        IdentConst*   constant;
        IdentVar*     variable;
        IdentArray*   array;
        IdentType*    type;
        IdentFunc*    function;
        IdentExpn*    expander;
        IdentInclude* include;
        IdentModule*  module;
    }id;
};

typedef struct IdentTableNode {
    Ident* id;
    int8   color;
    struct IdentTableNode* parent;
    struct IdentTableNode* lchild;
    struct IdentTableNode* rchild;
}IdentTableNode;

// the IdentTable is used to storage a set of information
// about nameable objects in C+ language.
struct IdentTable {
    IdentTableNode* root;
};

extern void   identTableInit   (IdentTable* id_table);
extern error  identTableAdd    (IdentTable* id_table, Ident* id);
extern Ident* identTableSearch (IdentTable* id_table, char*  id_name);
extern void   identTableDestroy(IdentTable* id_table);

#endif
