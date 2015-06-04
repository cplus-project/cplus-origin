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
#define ID_FUNC          0x05
#define ID_INCLUDE       0x06
#define ID_MODULE        0x07
#define ID_MICRO         0x08

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
typedef struct IdentInclude IdentInclude;
typedef struct IdentModule  IdentModule;
typedef struct IdentTable   IdentTable;

// represent a constant's information.
struct IdentConst {
    IdentType* instance;
};

// represent a variable's information.
struct IdentVar {
    IdentType* instance;
};

// represent an array's information.
struct IdentArray {
    IdentType* arr_datatype;
    int64      arr_len;
};

typedef struct PrimitiveType PrimitiveType;
typedef struct CompoundType  CompoundType;


// represent a primitive type of C+
// the primitive_type_token_code can be assigned with one of (see
// micro defined in lexer.h):
//    TOKEN_TYPE_BYTE
//    TOKEN_TYPE_INT8
//    TOKEN_TYPE_INT16
//    ...
//    TOKEN_TYPE_STRING
//
struct PrimitiveType {
    int16 type_token_code;
    bool  is_const;
    union {
        char*          value_const;
        PrimitiveType* value_var;
    }value;
};

// used to save the members' information of the type.
// the member_entity will be used base on the value of the
// member_type, the case of them is:
//    ID_CONST => member_entity.instance_const
//    ID_VAR   => member_entity.instance_var
//    ID_ARRAY => member_entity.instance_array
//    ID_TYPE  => member_entity.instance_type
typedef struct Member {
    int8  access;
    char* member_name;
    int8  member_type;
    union {
        IdentConst instance_const;
        IdentVar   instance_var;
        IdentArray instance_array;
        IdentType* instance_type;
    }member_entity;
    
    struct Member* next;
}Member;

// represent a compound type.
struct CompoundType {
    Member* member;
};

// represent a type definition's information.
// primitive:
//    true  => type_entity.instance_primitive
//    false => type_entity.instance_compound
struct IdentType {
    bool primitive;
    union {
        PrimitiveType instance_primitive;
        CompoundType  instance_compound;
    }type_entity;
};

typedef struct ParamListNode {
    char* param_type;
    char* param_name;
    struct ParamListNode* next;
}ParamListNode;

typedef struct {
    ParamListNode* head;
}ParamList;

// represent a function definition's information.
struct IdentFunc {
    ParamList passin;
    ParamList retout;
};

struct IdentInclude {
    IdentTable* id_table;
};

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
//    case ID_CONST:   // do some process to the 'id->id_info.id_const'
//    case ID_VAR:     // do some process to the 'id->id_info.id_var'
//    case ID_ARRAY:   // do some process to the 'id->id_info.id_array'
//    case ID_TYPE:    // do some process to the 'id->id_info.id_type'
//    case ID_FUNC:    // do some process to the 'id->id_info.id_func'
//    case ID_INCLUDE: // do some process to the 'id->id_info.id_include'
//    case ID_MODULE:  // do some process to the 'id->id_info.id_module'
//    case ID_MICRO: ...
//    default: ...
//    }
//    ...
struct Ident {
    char* id_name;
    int8  access;
    int8  id_type;
    union {
        IdentConst*   id_const;
        IdentVar*     id_var;
        IdentArray*   id_array;
        IdentType*    id_type;
        IdentFunc*    id_func;
        IdentInclude* id_include;
        IdentModule*  id_module;
    }id_entity;
};

extern Ident* makeIdentConst  (char* id_name, int8 access, IdentType* instance);
extern Ident* makeIdentVar    (char* id_name, int8 access, IdentType* instance);
extern Ident* makeIdentType   (char* id_name, int8 access, IdentType* typeinfo);
extern Ident* makeIdentFunc   (char* id_name, int8 access);
extern Ident* makeIdentInclude(char* id_name);
extern Ident* makeIdentModule (char* id_name);

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
