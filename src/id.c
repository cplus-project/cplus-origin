/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "id.h"

/****** methods of idtable ******/

void idtable_init(idtable* idt) {
    int i;
    for (i = 0; i < 16; i++) {
        idt->ids_head[i] = NULL;
    }
}

// how to get the hash value from the key:
// the raw key is the id's name. we explain the algorithm with
// an example. now we will see what the hash value for the id's
// name "req":
//   1. get the last and the middle letter, they are 'q' and 'e'.
//      we use the last and the middle one because we often name
//      our variables with the same prefix.
//   2. convert their ascii number to the binary format:
//      'q' -> 01110001
//      'e' -> 01100101
//   3. we will only use their rightmost two digit:
//      'q' -> xxxxxx01
//      'e' -> xxxxxx01
//      (we can get it by doing an &3 computing)
//   4. the hash value is:
//      (('q' & 3) << 2) + ('e' & 3) = 01 << 2 + 01 = 0101
//      0101 is 5 in decimal, so we use the 5 as the key to do
//      some operation to the id saved in the table.
int idtable_hash(char* key, int key_len) {
    return ((key[key_len-1]&3) << 2) + (key[key_len>>1]&3);
}

error idtable_insert(idtable* idt, id_info id) {
    if (id.id_name == NULL || id.id_len <= 0 || id.id_type == ID_UNKNOWN) {
        return new_error("err: can not insert an invalid id.");
    }
    int index = idtable_hash(id.id_name, id.id_len);
    if (idt->ids_head[index] == NULL) {
        idt->ids_head[index] = (idtable_node*)mem_alloc(sizeof(idtable_node));
        idt->ids_head[index]->id   = id;
        idt->ids_head[index]->next = NULL;
        return NULL;
    }
    else {
        idtable_node* ptr = idt->ids_head[index];
        for (;;) {
            if (ptr->next == NULL) {
                ptr->next = (idtable_node*)mem_alloc(sizeof(idtable_node));
                ptr->next->id   = id;
                ptr->next->next = NULL;
                return NULL;
            }
            ptr = ptr->next;
        }
    }
}

error idtable_update(idtable* idt, id_info new_info) {
    if (new_info.id_name == NULL || new_info.id_len <= 0 || new_info.id_type == ID_UNKNOWN) {
        return new_error("err: can not be update to an invalid id.");
    }
    int i;
    int index = idtable_hash(new_info.id_name, new_info.id_len);
    idtable_node* ptr = NULL;
    for (ptr = idt->ids_head[index]; ptr != NULL; ptr = ptr->next) {
        if (ptr->id.id_len == new_info.id_len) {
            for (i = 0; i < ptr->id.id_len; i++) {
                if (ptr->id.id_name[i] != new_info.id_name[i]) {
                    break;
                }
            }
            ptr->id = new_info;
            return NULL;
        }
    }
    return new_error("err: the id entry not found.");
}

error idtable_search(idtable* idt, id_info* ret) {
    if (ret->id_name == NULL || ret->id_len <= 0) {
        return new_error("err: can not search the id through an invalid id name.");
    }
    int i;
    int index = idtable_hash(ret->id_name, ret->id_len);
    idtable_node* ptr = NULL;
    for (ptr = idt->ids_head[index]; ptr != NULL; ptr = ptr->next) {
        if (ptr->id.id_len == ret->id_len) {
            for (i = 0; i < ptr->id.id_len; i++) {
                if (ptr->id.id_name[i] != ret->id_name[i]) {
                    break;
                }
            }
            *ret = ptr->id;
            return NULL;
        }
    }
    return new_error("err: id not found.");
}

void idtable_destroy(idtable* idt) {
    int i;
    idtable_node* ptr = NULL;
    for (i = 0; i < 16; i++) {
        for (ptr = idt->ids_head[i]; ptr != NULL; ptr = ptr->next) {
            mem_free(ptr);
        }
    }
}

void idtable_debug(idtable* idt) {
    if (idt->ids_head == NULL) {
        assert("the id table is not initialized");
    }
    int i;
    idtable_node* ptr = NULL;
    for (i = 0; i < 16; i++) {
        printf("id_node #%02d:\r\n", i+1);
        for (ptr = idt->ids_head[i]; ptr != NULL; ptr = ptr->next) {
            if (ptr->id.id_name != NULL)
                printf("\t%s  ", ptr->id.id_name);
            else
                printf("\tnone  ");

            switch (ptr->id.id_type) {
            case ID_CONST:
                printf("const  ");
                break;
            case ID_VAR:
                printf("variable  ");
                break;
            case ID_MOD:
                printf("modular  ");
                break;
            }

            if (ptr->id.id_datatype != NULL)
                printf("%s  ", ptr->id.id_datatype);
            else
                printf("none  ");

            if (ptr->id.id_value != NULL)
                printf("%s\r\n", ptr->id.id_value);
            else
                printf("none\r\n");
        }
    }
}

/****** methods of decl_list ******/

void decl_list_init(decl_list* declist) {
    declist->head = NULL;
    declist->cur  = NULL;
}

void decl_list_add(decl_list* declist, declare decl) {
    decl_list_node* create = (decl_list_node*)mem_alloc(sizeof(decl_list_node));
    create->decl = decl;
    create->next = NULL;
    if (declist->head != NULL) {
        declist->cur->next = create;
        declist->cur       = create;
    }
    else {
        declist->head = create;
        declist->cur  = create;
    }
}

void decl_list_destroy(decl_list* declist) {
    decl_list_node* ptr = declist->head;
    decl_list_node* temp;
    for (;;) {
        if (ptr == NULL) {
            return;
        }
        temp = ptr;
        ptr  = ptr->next;
        mem_free(temp);
    }
    declist->head = NULL;
    declist->cur  = NULL;
}

void decl_list_debug(decl_list* declist) {
    decl_list_node* ptr;
    printf("all nodes in the declare list is:\r\n");
    if (declist->head == NULL) {
        printf("none nodes\r\n");
        return;
    }
    printf("column1->type  column2->name  column3->assign\r\n");
    printf("%s %s %s <-top", declist->head->decl.decl_type, declist->head->decl.decl_name, declist->head->decl.decl_assign);
    for (ptr = declist->head->next; ptr != NULL; ptr = ptr->next) {
        printf("\r\n%s %s %s", ptr->decl.decl_type, ptr->decl.decl_name, ptr->decl.decl_assign);
    }
    printf(" <-cur\r\n");
}
