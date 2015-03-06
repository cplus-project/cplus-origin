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
error type_table_search(type_table* typetab, type* search) {
    type_table_node* ptr = typetab->root;
    for (;;) {
        switch (type_table_cmp(search, &ptr->typeinfo)) {
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
            search->type_access     = ptr->typeinfo.type_access;
            search->type_properties = ptr->typeinfo.type_properties;
            search->type_methods    = ptr->typeinfo.type_methods;
            return NULL;
        default:
            return new_error("err: can not compare the two type names.");
        }
    }
}

// use the postorder to travel the tree and release
// every node because this order will not need for
// extra visited-flag.
void type_table_destroy(type_table* typetab) {
    type_table_iterator it;
    type_table_iterator_init(&it);
    type_table_node* ptr = typetab->root;
    for (;;) {
        if (ptr->lchild != NULL) {
            type_table_iterator_push(&it, ptr);
            ptr = ptr->lchild;
        }
        else if (ptr->rchild != NULL) {
            ptr = ptr->rchild;
        }
        else {
            if (ptr == ptr->parent->lchild) {
                ptr->parent->lchild = NULL;
                mem_free(ptr);
                ptr = type_table_iterator_top(&it);
            }
            else {
                ptr->parent->rchild = NULL;
                mem_free(ptr);
                ptr = type_table_iterator_top(&it);
                type_table_iterator_pop(&it);
            }
        }
    }
    type_table_iterator_destroy(&it);
}

/****** methods of type_table_iterator ******/

void type_table_iterator_init(type_table_iterator* it) {
    it->top = NULL;
}

void type_table_iterator_push(type_table_iterator* it, type_table_node* log) {
    type_table_iterator_node* create = (type_table_iterator_node*)mem_alloc(sizeof(type_table_iterator_node));
    create->log  = log;
    create->next = NULL;
    if (it->top != NULL) {
        create->next = it->top;
        it->top = create;
    }
    else {
        it->top = create;
    }
}

type_table_node* type_table_iterator_top(type_table_iterator* it) {
    if (it->top == NULL) {
        return NULL;
    }
    return it->top->log;
}

error type_table_iterator_pop(type_table_iterator* it) {
    if (it->top != NULL) {
        type_table_iterator_node* temp = it->top;
        it->top = it->top->next;
        mem_free(temp);
    }
    else {
        return new_error("err: the type table iterator is empty.");
    }
}

void type_table_iterator_destroy(type_table_iterator* it) {
    type_table_iterator_node* temp;
    for (;;) {
        if (it->top == NULL) {
            return;
        }
        temp = it->top;
        it->top = it->top->next;
        mem_free(temp);
    }
}

void type_table_iterator_debug(type_table_iterator* it) {
    printf("all nodes in the type table iterator:\r\n");
    if (it->top == NULL) {
        printf("none node\r\n");
        return;
    }
    printf("%s <-top\r\n", it->top->log->typeinfo.type_name);
    type_table_iterator_node* ptr;
    for (ptr = it->top->next; ptr != NULL; ptr = ptr->next) {
        printf("%s\r\n", ptr->log->typeinfo.type_name);
    }
}
