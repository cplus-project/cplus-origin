/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "idtable.h"

void idtable_init(idtable* idt) {
    int i;
    for (i = 0; i < 16; i++) {
        idt->ids_head[i] = NULL;
    }
}

error idtable_insert(idtable* idt, id_info id) {
    if (id.id_name == NULL || id.id_len <= 0 || id.id_type == ID_UNKNOWN) {
        return new_error("err: can not insert an invalid id.");
    }
    // how to get the hash value from the key:
    // the raw key is the id's name. we explain the algorithm with
    // an example. now we will see what the hash value for the id's
    // name "req":
    // 1. get the last and the first letter, they are 'q' and 'r'.
    // 2. convert their ascii number to the binary format:
    //    'q' -> 01110001
    //    'r' -> 01110010
    // 3. we will only use their rightmost two digit:
    //    'q' -> xxxxxx01
    //    'r' -> xxxxxx10
    //    (we can get it by doing an &3 computing)
    // 4. the hash value is:
    //    01 << 2 + 10 = 0110
    //    0110 is 6 in decimal, so we use the 6 as the key to do
    //    some operation to the id saved in the table.
    int index = ((id.id_name[id.id_len-1]&3) << 2) + (id.id_name[0]&3);
    if (idt->ids_head[index] == NULL) {
        idt->ids_head[index] = (idtable_node*)mem_alloc(sizeof(idtable_node));
        idt->ids_head[index]->id   = id;
        idt->ids_head[index]->next = NULL;
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
    int index = ((new_info.id_name[new_info.id_len-1]&3) << 2) + (new_info.id_name[0]&3);
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
    int index = ((ret->id_name[ret->id_len-1]&3)<<2) + (ret->id_name[0]&3);
    idtable_node* ptr = NULL;
    for (ptr = idt->ids_head[index]; ptr != NULL; ptr = ptr->next) {
        if (ptr->id.id_len == ret->id_len) {
            for (i = 0; i < ptr->id.id_len; i++) {
                if (ptr->id.id_name[i] != ret->id_name[i]) {
                    break;
                }
            }
            ret = &ptr->id;
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
