/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "ident.h"

static Ident id_placeholder = {NULL, ACCESS_IN, ID_PLACEHOLDER, NULL};

Ident* makeIdentconst(char* id_name, int8 access, IdentType* instance) {
    Ident* id = (Ident*)mem_alloc(sizeof(Ident));
    id->id_name = id_name;
    id->access  = access;
    id->id_type = ID_CONST;
    id->id_entity.id_const = (IdentConst*)mem_alloc(sizeof(IdentConst));
    id->id_entity.id_const->instance = instance;
    return id;
}

Ident* makeIdentVar(char* id_name, int8 access, IdentType* instance) {
    Ident* id = (Ident*)mem_alloc(sizeof(Ident));
    id->id_name = id_name;
    id->access  = access;
    id->id_type = ID_VAR;
    id->id_entity.id_var = (IdentVar*)mem_alloc(sizeof(IdentVar));
    id->id_entity.id_var->instance = instance;
    return id;
}

Ident* makeIdentType(char* id_name, int8 access, IdentType* typeinfo) {
    Ident* id = (Ident*)mem_alloc(sizeof(Ident));
    id->id_name = id_name;
    id->access  = access;
    id->id_type = ID_TYPE;
    id->id_entity.id_type = typeinfo;
    return id;
}

Ident* makeIdentFunc(char* id_name, int8 access) {
    Ident* id = (Ident*)mem_alloc(sizeof(Ident));
    id->id_name = id_name;
    id->access  = access;
    id->id_type = ID_FUNC;
    id->id_entity.id_func = (IdentFunc*)mem_alloc(sizeof(IdentFunc));
    id->id_entity.id_func->passin.head = NULL;
    id->id_entity.id_func->retout.head = NULL;
    return id;
}

Ident* makeIdentInclude(char* id_name) {
    Ident* id = (Ident*)mem_alloc(sizeof(Ident));
    id->id_name = id_name;
    id->access  = ACCESS_NULL;
    id->id_type = ID_INCLUDE;
    id->id_entity.id_include = (IdentInclude*)mem_alloc(sizeof(IdentInclude));
    id->id_entity.id_include->id_table = (IdentTable*)mem_alloc(sizeof(IdentTable));
    identTableInit(id->id_entity.id_include->id_table);
    return id;
}

Ident* makeIdentModule(char* id_name) {
    Ident* id = (Ident*)mem_alloc(sizeof(Ident));
    id->id_name = id_name;
    id->access  = ACCESS_NULL;
    id->id_type = ID_MODULE;
    id->id_entity.id_module = (IdentModule*)mem_alloc(sizeof(IdentModule));
    id->id_entity.id_module->id_table = (IdentTable*)mem_alloc(sizeof(IdentTable));
    identTableInit(id->id_entity.id_module->id_table);
    return id;
}

/****** methods of IdentTable ******/

void identTableInit(IdentTable* id_table) {
    id_table->root = NULL;
}

// firstly compare every character of the two name from index 0:
// (1) if name1[index] < name2[index] -> NODE_CMP_LT.
// (2) if name1[index] > name2[index] -> NODE_CMP_GT.
//
// if all characters compared are the same, then compare their names' length:
// (1) if the name1's length is longer , return NODE_CMP_GT.
// (2) if the name1's length is shorter, return NODE_CMP_LT.
// (3) if tow names'  length are equal , return NODE_CMP_EQ.
static int identTableCmp(char* name1, char* name2) {
    int64 i;
    for (i = 0; ; i++) {
        if (name1[i] < name2[i]) {
            return NODE_CMP_LT;
        }
        if (name1[i] > name2[i]) {
            return NODE_CMP_GT;
        }
        if (name1[i] == '\0' && name2[i] == '\0') {
            return NODE_CMP_EQ;
        }
        if (name1[i] == '\0' && name2[i] != '\0') {
            return NODE_CMP_LT;
        }
        if (name1[i] != '\0' && name2[i] == '\0') {
            return NODE_CMP_GT;
        }
    }
}

// example:
//     |                       |
//    node                   rchild
//   /    \      ----\      /      \
//  a   rchild   ----/    node      c
//     /      \          /    \
//    b        c        a      b
static error identTableLeftRotate(IdentTable* id_table, IdentTableNode* node) {
    if (node == id_table->root) {
        id_table->root = node->rchild;
        node->rchild->parent = NULL;
    }
    else {
        (node == node->parent->lchild)?
        (node->parent->lchild = node->rchild):
        (node->parent->rchild = node->rchild);
         node->rchild->parent = node->parent;
    }
    node->parent = node->rchild;
    node->rchild = node->rchild->lchild;
    if (node->parent->lchild != NULL) {
        node->parent->lchild->parent = node;
    }
    node->parent->lchild = node;
}

// example:
//        |                |
//       node            lchild
//      /    \  ----\   /      \
//   lchild   c ----/  a      node
//  /      \                 /    \
// a        b               b      c
static error identTableRightRotate(IdentTable* id_table, IdentTableNode* node) {
    if (node == id_table->root) {
        id_table->root = node->lchild;
        node->lchild->parent = NULL;
    }
    else {
        (node == node->parent->lchild)?
        (node->parent->lchild = node->lchild):
        (node->parent->rchild = node->lchild);
         node->lchild->parent = node->parent;
    }
    node->parent = node->lchild;
    node->lchild = node->lchild->rchild;
    if (node->parent->rchild != NULL) {
        node->parent->rchild->parent = node;
    }
    node->parent->rchild = node;
}

// fix the balance of the tree and try to keep the properties
// of the red black tree.
static void identTableAddFixup(IdentTable* id_table, IdentTableNode* added) {
    IdentTableNode* uncle = NULL;
    for (;;) {
        if (added->parent == NULL || added->parent->parent == NULL)
            break;

        if (added->color == NODE_COLOR_RED && added->parent->color == NODE_COLOR_RED) {
            // the uncle is black or NULL
            if (added->parent == added->parent->parent->lchild) {
                uncle = added->parent->parent->rchild;
                if (uncle == NULL || uncle->color == NODE_COLOR_BLACK) {
                    if (added == added->parent->rchild) {
                        added =  added->parent;
                        identTableLeftRotate(id_table, added);
                    }
                    added->parent->color         = NODE_COLOR_BLACK;
                    added->parent->parent->color = NODE_COLOR_RED;
                    identTableRightRotate(id_table, added->parent->parent);
                    break;
                }
            }
            else {
                uncle = added->parent->parent->lchild;
                if (uncle == NULL || uncle->color == NODE_COLOR_BLACK) {
                    if (added == added->parent->lchild) {
                        added =  added->parent;
                        identTableRightRotate(id_table, added);
                    }
                    added->parent->color         = NODE_COLOR_BLACK;
                    added->parent->parent->color = NODE_COLOR_RED;
                    identTableLeftRotate(id_table, added->parent->parent);
                    break;
                }
            }

            // the uncle is red
            uncle->color         = NODE_COLOR_BLACK;
            uncle->parent->color = NODE_COLOR_RED;
            added->parent->color = NODE_COLOR_BLACK;
            added = added->parent->parent;
        }
        else {
            break;
        }
    }
    id_table->root->color = NODE_COLOR_BLACK;
}

error identTableAdd(IdentTable* id_table, Ident* id) {
    if (id->id_name == NULL) {
        return new_error("the identifier's name can not be NULL.");
    }
    IdentTableNode* create = (IdentTableNode*)mem_alloc(sizeof(IdentTableNode));
    create->id       = id;
    create->color    = NODE_COLOR_RED;
    create->parent   = NULL;
    create->lchild   = NULL;
    create->rchild   = NULL;
    if (id_table->root != NULL) {
        IdentTableNode* ptr = id_table->root;
        for (;;) {
            switch (identTableCmp(id->id_name, ptr->id->id_name)) {
            case NODE_CMP_LT:
                if (ptr->lchild == NULL) {
                    ptr->lchild = create;
                    create->parent = ptr;
                    identTableAddFixup(id_table, create);
                    return NULL;
                }
                ptr = ptr->lchild;
                break;
            case NODE_CMP_GT:
                if (ptr->rchild == NULL) {
                    ptr->rchild = create;
                    create->parent = ptr;
                    identTableAddFixup(id_table, create);
                    return NULL;
                }
                ptr = ptr->rchild;
                break;
            case NODE_CMP_EQ:
                return new_error("err: identifier redefined.");
            default:
                return new_error("err: can not compare the two id names.");
            }
        }
    }
    else {
        create->color = NODE_COLOR_BLACK;
        id_table->root = create;
        return NULL;
    }
}

// return:
//       NULL -> the identifier is not in the table.
//   NOT NULL -> the identifier is found.
Ident* identTableSearch(IdentTable* id_table, char* id_name) {
    if (id_table->root == NULL) {
        return NULL;
    }
    if (strcmp(id_name, "_") == 0) {
        return &id_placeholder;
    }
    IdentTableNode* ptr = id_table->root;
    for (;;) {
        switch (identTableCmp(id_name, ptr->id->id_name)) {
        case NODE_CMP_LT:
            if (ptr->lchild != NULL) {
                ptr = ptr->lchild;
                break;
            }
            return NULL;

        case NODE_CMP_GT:
            if (ptr->rchild != NULL) {
                ptr = ptr->rchild;
                break;
            }
            return NULL;

        case NODE_CMP_EQ:
            return ptr->id;

        default:
            return NULL;
        }
    }
}

static void identTableDestroyNode(IdentTableNode* node) {
    if (node != NULL) {
        if (node->lchild != NULL) identTableDestroyNode(node->lchild);
        if (node->rchild != NULL) identTableDestroyNode(node->rchild);
        switch (node->id->id_type) {
        case ID_TYPE:
            if (node->id->id_entity.id_type->primitive == false) {
                Member* temp;
                Member* ptr = node->id->id_entity.id_type->type_entity.instance_compound.member;
                for (;;) {
                    if (ptr == NULL) {
                        break;
                    }
                    temp = ptr;
                    ptr  = ptr->next;
                    mem_free(temp);
                }
            }
            break;
        case ID_FUNC: {
                ParamListNode* del = NULL;
                ParamListNode* ptr = node->id->id_entity.id_func->passin.head;
                while (ptr != NULL) {
                    del = ptr;
                    ptr = ptr->next;
                    mem_free(del);
                }
                for (ptr = node->id->id_entity.id_func->retout.head; ptr != NULL;) {
                    del = ptr;
                    ptr = ptr->next;
                    mem_free(del);
                }
                node->id->id_entity.id_func->passin.head = NULL;
                node->id->id_entity.id_func->retout.head = NULL;
            }
            break;
        case ID_INCLUDE:
            identTableDestroy(node->id->id_entity.id_include->id_table);
            break;
        case ID_MODULE:
            identTableDestroy(node->id->id_entity.id_module->id_table);
            break;
        }
        mem_free(node->id);
        mem_free(node);
    }
}

void identTableDestroy(IdentTable* id_table) {
    identTableDestroyNode(id_table->root);
}
