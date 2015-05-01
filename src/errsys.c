/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "errsys.h"

/****** methods of err_info_log ******/

void err_info_log_init(err_info_log* log) {
    log->head = NULL;
    log->tail = NULL;
}

void err_info_log_add(err_info_log* log, bool must, char* file_name, char* func_name, int32 line, int16 col) {
    err_info_log_node* create = (err_info_log_node*)mem_alloc(sizeof(err_info_log_node));
    create->info.must_deal = must;
    create->info.file_name = file_name;
    create->info.func_name = func_name;
    create->info.line      = line;
    create->info.col       = col;
    create->next           = NULL;
    if (log->head != NULL) {
        log->tail->next = create;
        log->tail = create;
    }
    else {
        log->head = create;
        log->tail = create;
    }
}

void err_info_log_clear(err_info_log* log) {
    err_info_log_node* temp = NULL;
    for (;;) {
        if (log->head == NULL) {
            log->tail =  NULL;
            return;
        }
        temp = log->head;
        log->head = log->head->next;
        mem_free(temp);
    }
}

void err_info_log_destroy(err_info_log* log) {
    err_info_log_clear(log);
}

/****** methods of err_table ******/

void err_table_init(err_table* etab) {
    etab->root       = NULL;
    etab->err_count  = 0;
    etab->must_count = 0;
}

// firstly compare every character of the two name from index 0:
// (1) if name1[index] < name2[index] -> NODE_CMP_LT.
// (2) if name1[index] > name2[index] -> NODE_CMP_GT.
//
// if all characters compared are the same, then compare their names' length:
// (1) if the name1's length is longer , return NODE_CMP_GT.
// (2) if the name1's length is shorter, return NODE_CMP_LT.
// (3) if tow names'  length are equal , return NODE_CMP_EQ.
static int err_table_cmp(char* name1, char* name2) {
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
static error err_table_left_rotate(err_table* etab, err_table_node* node) {
    if (node == etab->root) {
        etab->root = node->rchild;
        node->rchild->parent = NULL;
    }
    else {
        node == node->parent->lchild ? (node->parent->lchild = node->rchild) : (node->parent->rchild = node->rchild);
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
static error err_table_right_rotate(err_table* etab, err_table_node* node) {
    if (node == etab->root) {
        etab->root = node->lchild;
        node->lchild->parent = NULL;
    }
    else {
        node == node->parent->lchild ? (node->parent->lchild = node->lchild) : (node->parent->rchild = node->lchild);
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
static void err_table_add_fixup(err_table* etab, err_table_node* added) {
    err_table_node* uncle = NULL;
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
                        err_table_left_rotate(etab, added);
                    }
                    added->parent->color         = NODE_COLOR_BLACK;
                    added->parent->parent->color = NODE_COLOR_RED;
                    err_table_right_rotate(etab, added->parent->parent);
                    break;
                }
            }
            else {
                uncle = added->parent->parent->lchild;
                if (uncle == NULL || uncle->color == NODE_COLOR_BLACK) {
                    if (added == added->parent->lchild) {
                        added =  added->parent;
                        err_table_right_rotate(etab, added);
                    }
                    added->parent->color         = NODE_COLOR_BLACK;
                    added->parent->parent->color = NODE_COLOR_RED;
                    err_table_left_rotate(etab, added->parent->parent);
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
    etab->root->color = NODE_COLOR_BLACK;
}

error err_table_add(err_table* etab, char* err_tag, bool must, char* file_name, char* func_name, int32 line, int16 col) {
    if (err_tag == NULL) {
        return new_error("the error tag can not be NULL.");
    }
    if (etab->root != NULL) {
        err_table_node* ptr = etab->root;
        for (;;) {
            switch (err_table_cmp(err_tag, ptr->err.err_tag)) {
            case NODE_CMP_LT:
                if (ptr->lchild == NULL) {
                    err_table_node* create = (err_table_node*)mem_alloc(sizeof(err_table_node));
                    create->err.err_tag   = err_tag;
                    create->err.err_count = 1;
                    create->err.is_dealt  = false;
                    create->color         = NODE_COLOR_RED;
                    create->parent        = NULL;
                    create->lchild        = NULL;
                    create->rchild        = NULL;
                    must == true ? (create->err.must_count = 1) : (create->err.must_count = 0);
                    err_info_log_init(&create->err.info_log);
                    err_info_log_add(&create->err.info_log, must, file_name, func_name, line, col);

                    ptr->lchild = create;
                    create->parent = ptr;
                    err_table_add_fixup(etab, create);
                    return NULL;
                }
                ptr = ptr->lchild;
                break;
            case NODE_CMP_GT:
                if (ptr->rchild == NULL) {
                    err_table_node* create = (err_table_node*)mem_alloc(sizeof(err_table_node));
                    create->err.err_tag   = err_tag;
                    create->err.err_count = 1;
                    create->err.is_dealt  = false;
                    create->color         = NODE_COLOR_RED;
                    create->parent        = NULL;
                    create->lchild        = NULL;
                    create->rchild        = NULL;
                    must == true ? (create->err.must_count = 1) : (create->err.must_count = 0);
                    err_info_log_init(&create->err.info_log);
                    err_info_log_add(&create->err.info_log, must, file_name, func_name, line, col);

                    ptr->rchild = create;
                    create->parent = ptr;
                    err_table_add_fixup(etab, create);
                    return NULL;
                }
                ptr = ptr->rchild;
                break;
            case NODE_CMP_EQ:
                if (ptr->err.is_dealt == true) {
                    ptr->err.is_dealt = false;
                }
                ptr->err.err_count++;
                if (must == true) {
                    ptr->err.must_count++;
                }
                err_info_log_add(&ptr->err.info_log, must, file_name, func_name, line, col);
                break;

            default:
                return new_error("err: can not compare the two error tags.");
            }
        }
    }
    else {
        err_table_node* create = (err_table_node*)mem_alloc(sizeof(err_table_node));
        create->err.err_tag   = err_tag;
        create->err.err_count = 1;
        create->err.is_dealt  = false;
        create->color         = NODE_COLOR_BLACK;
        create->parent        = NULL;
        create->lchild        = NULL;
        create->rchild        = NULL;
        must == true ? (create->err.must_count = 1) : (create->err.must_count = 0);
        err_info_log_init(&create->err.info_log);
        err_info_log_add(&create->err.info_log, must, file_name, func_name, line, col);
        etab->root = create;
        return NULL;
    }

    etab->err_count++;
    if (must == true) {
        etab->must_count++;
    }
    return NULL;
}

static void err_table_clean(err_table* etab, err_table_node* node) {
    etab->err_count  -= node->err.err_count;
    etab->must_count -= node->err.must_count;
    node->err.err_count  = 0;
    node->err.must_count = 0;
    node->err.is_dealt   = true;
    err_info_log_clear(&node->err.info_log);
}

bool err_table_rm(err_table* etab, char* err_tag) {
    if (etab->root == NULL) {
        return false;
    }
    err_table_node* ptr = etab->root;
    for (;;) {
        switch (err_table_cmp(err_tag, ptr->err.err_tag)) {
        case NODE_CMP_LT:
            if (ptr->lchild != NULL)
                ptr = ptr->lchild;
            else
                return false;
            break;
        case NODE_CMP_GT:
            if (ptr->rchild != NULL)
                ptr = ptr->rchild;
            else
                return false;
            break;
        case NODE_CMP_EQ:
            err_table_clean(etab, ptr);
            return true;

        default:
            return false;
        }
    }
}

#define WILDCARD_MATCH_LT 0x00
#define WILDCARD_MATCH_OK 0x01
#define WILDCARD_MATCH_GT 0x02

static int err_table_wdcd_match(char* tag, char* tag_wdcd) {
    int64 i;
    for (i = 0;; i++) {
        if (tag[i] < tag_wdcd[i])
            return WILDCARD_MATCH_LT;
        if (tag[i] > tag_wdcd[i])
            return WILDCARD_MATCH_GT;
        if (tag[i] != '\0' && tag_wdcd[i] == '\0' || tag[i] == '\0' && tag_wdcd[i] == '\0')
            return WILDCARD_MATCH_OK;
    }
}

static void err_table_wdcd_clean(err_table* etab, err_table_node* node, char* err_tag_wdcd) {
    if (node != NULL) {
        if (node->lchild != NULL) {
            err_table_wdcd_clean(etab, node->lchild, err_tag_wdcd);
        }
        if (node->err.is_dealt == false && err_table_wdcd_match(node->err.err_tag, err_tag_wdcd) == WILDCARD_MATCH_OK) {
            err_table_clean(etab, node);
        }
        if (node->rchild != NULL) {
            err_table_wdcd_clean(etab, node->rchild, err_tag_wdcd);
        }
    }
}

bool err_table_rm_wdcd(err_table* etab, char* err_tag_wdcd) {
    if (etab->root == NULL) {
        return false;
    }
    err_table_node* ptr = etab->root;
    for (;;) {
        switch (err_table_wdcd_match(ptr->err.err_tag, err_tag_wdcd)) {
        case WILDCARD_MATCH_LT:
            if (ptr->lchild != NULL)
                ptr = ptr->lchild;
            else
                return false;
            break;
        case WILDCARD_MATCH_GT:
            if (ptr->rchild != NULL)
                ptr = ptr->rchild;
            else
                return false;
            break;
        case WILDCARD_MATCH_OK:
            err_table_clean(etab, ptr);
            err_table_wdcd_clean(etab, ptr, err_tag_wdcd);
            break;

        default:
            return false;
        }
    }
}

static void err_table_destroy_node(err_table_node* node) {
    if (node != NULL) {
        if (node->lchild != NULL) err_table_destroy_node(node->lchild);
        if (node->rchild != NULL) err_table_destroy_node(node->rchild);
        err_info_log_destroy(&node->err.info_log);
        mem_free(node);
    }
}

void err_table_destroy(err_table* etab) {
    err_table_destroy_node(etab->root);
}
