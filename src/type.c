/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "type.h"

/****** methods of type ******/

void type_init(type* typ) {
    typ->type_access  = TYPE_ACCESS_IN;
    typ->type_name    = NULL;
    typ->type_namelen = 0;
    decl_list_init(&typ->type_properties);
    func_table_init(&typ->type_methods);
}

void type_destroy(type* typ) {
    decl_list_destroy(&typ->type_properties);
    func_table_destroy(&typ->type_methods);
}

/****** methods of type_table ******/

void type_table_init(type_table* typetab) {
    typetab->root = NULL;
}

int type_table_cmp(type* t1, type* t2) {
    int i = 0;
    // compare every character of the two typename from index 0.
    // if t1's typename[index] < t2's typename[index] -> NODE_CMP_LT.
    // if t1's typename[index] > t2's typename[index] -> NODE_CMP_GT.
    // if all characters compared are the same, then compare their
    // typenames' length. if the t1's length is longer, return
    // NODE_CMP_GT, if the t1's length is equal to the t2's length,
    // then return NODE_CMP_EQ.
    if (t1->type_namelen < t2->type_namelen) {
        for (i = 0; i < t1->type_namelen; i++) {
            if (t1->type_name[i] < t2->type_name[i]) return NODE_CMP_LT;
            if (t1->type_name[i] > t2->type_name[i]) return NODE_CMP_GT;
        }
        return NODE_CMP_LT;
    }
    else if (t1->type_namelen > t2->type_namelen) {
        for (i = 0; i < t2->type_namelen; i++) {
            if (t1->type_name[i] < t2->type_name[i]) return NODE_CMP_LT;
            if (t1->type_name[i] > t2->type_name[i]) return NODE_CMP_GT;
        }
        return NODE_CMP_GT;
    }
    else {
        for (i = 0; i < t2->type_namelen; i++) {
            if (t1->type_name[i] < t2->type_name[i]) return NODE_CMP_LT;
            if (t1->type_name[i] > t2->type_name[i]) return NODE_CMP_GT;
        }
        return NODE_CMP_EQ;
    }
}

// note:
//   the typeinfo should be initialized with type_init()
//   before being assigned and being passed into the
//   type_table_add().
error type_table_add(type_table* typetab, type typeinfo) {
    if (typeinfo.type_name == NULL) {
        return new_error("err: the type's name can not be NULL.");
    }
    type_table_node* create = (type_table_node*)mem_alloc(sizeof(type_table_node));
    create->typeinfo = typeinfo;
    create->parent   = NULL;
    create->lchild   = NULL;
    create->rchild   = NULL;
    if (typetab->root != NULL) {
        type_table_node* ptr = typetab->root;
        for (;;) {
            switch (type_table_cmp(&typeinfo, &ptr->typeinfo)) {
            case NODE_CMP_LT:
                if (ptr->lchild == NULL) {
                    ptr->lchild = create;
                    create->parent = ptr;
                    return NULL;
                }
                ptr = ptr->lchild;
                break;
            case NODE_CMP_GT:
                if (ptr->rchild == NULL) {
                    ptr->rchild = create;
                    create->parent = ptr;
                    return NULL;
                }
                ptr = ptr->rchild;
                break;
            case NODE_CMP_EQ:
                return new_error("err: type redefined.");
            default:
                return new_error("err: can not compare the two type names.");
            }
        }
    }
    else {
        create->color = NODE_COLOR_BLACK;
        typetab->root = create;
    }
}

// note:
//   you should assign the ret->type_name and ret->type_namelen
//   when you want to search a type from the table.
// an error will returned if the type is not in the table or
// some errors occur.
error type_table_search(type_table* typetab, type* ret) {
    type_table_node* ptr = typetab->root;
    for (;;) {
        switch (type_table_cmp(ret, &ptr->typeinfo)) {
        case NODE_CMP_LT:
            if (ptr->lchild != NULL) {
                ptr = ptr->lchild;
            }
            else {
                return new_error("err: the type searched not found.");
            }
            break;
        case NODE_CMP_GT:
            if (ptr->rchild != NULL) {
                ptr = ptr->rchild;
            }
            else {
                return new_error("err: the type searched not found.");
            }
            break;
        case NODE_CMP_EQ:
            ret->type_access     = ptr->typeinfo.type_access;
            ret->type_properties = ptr->typeinfo.type_properties;
            ret->type_methods    = ptr->typeinfo.type_methods;
            return NULL;
        default:
            return new_error("err: can not compare the two type names.");
        }
    }
}
