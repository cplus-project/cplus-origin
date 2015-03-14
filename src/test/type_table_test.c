/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The test for type.h and type.c
 **/

#include "string.h"
#include "../type.h"

void type_table_rec_black_count(type_table_node* node, int counted) {
    if (node != NULL) {
        if (node->color == NODE_COLOR_BLACK) {
            counted++;
        }
        if (node->lchild != NULL) type_table_rec_black_count(node->lchild, counted);
        if (node->rchild != NULL) type_table_rec_black_count(node->rchild, counted);
        if (node->lchild == NULL && node->rchild == NULL) {
            printf("%d\r\n", counted);
        }
    }
}

int type_table_deepth(type_table_node* node) {
    int ldeepth = 0;
    int rdeepth = 0;
    if (node == NULL) return 0;
    if (node->lchild == NULL && node->rchild == NULL) return 1;
    if (node->lchild != NULL) ldeepth += type_table_deepth(node->lchild);
    if (node->rchild != NULL) rdeepth += type_table_deepth(node->rchild);
    return ldeepth > rdeepth ? ldeepth+1 : rdeepth+1;
}

void type_table_preorder_travel(type_table_node* node) {
    if (node != NULL) {
        printf("%s ", node->typeinfo.type_name);
        if (node->lchild != NULL) type_table_preorder_travel(node->lchild);
        if (node->rchild != NULL) type_table_preorder_travel(node->rchild);
    }
}

void type_table_inorder_travel(type_table_node* node) {
    if (node != NULL) {
        if (node->lchild != NULL) type_table_inorder_travel(node->lchild);
        printf("%s ", node->typeinfo.type_name);
        if (node->rchild != NULL) type_table_inorder_travel(node->rchild);
    }
}

void type_table_postorder_travel(type_table_node* node) {
    if (node != NULL) {
        if (node->lchild != NULL) type_table_postorder_travel(node->lchild);
        if (node->rchild != NULL) type_table_postorder_travel(node->rchild);
        printf("%s ", node->typeinfo.type_name);
    }
}

int main() {
    int   i;
    error err = NULL;
    type  t[10];
    for (i = 0; i < 10; i++) {
        type_init(&t[i]);
        t[i].type_access = TYPE_ACCESS_OUT;
    }

    t[0].type_name    = "createTree";
    t[0].type_namelen = strlen(t[0].type_name);

    t[1].type_name    = "travelGraph";
    t[1].type_namelen = strlen(t[1].type_name);

    t[2].type_name    = "addChild";
    t[2].type_namelen = strlen(t[2].type_name);

    t[3].type_name    = "sum";
    t[3].type_namelen = strlen(t[3].type_name);

    t[4].type_name    = "subString";
    t[4].type_namelen = strlen(t[4].type_name);

    t[5].type_name    = "personSay";
    t[5].type_namelen = strlen(t[5].type_name);

    t[6].type_name    = "personWalk";
    t[6].type_namelen = strlen(t[6].type_name);

    t[7].type_name    = "init";
    t[7].type_namelen = strlen(t[7].type_name);

    t[8].type_name    = "add";
    t[8].type_namelen = strlen(t[8].type_name);

    t[9].type_name    = "showMessage";
    t[9].type_namelen = strlen(t[9].type_name);

    type_table typetab;
    type_table_init(&typetab);
    // add test
    for (i = 0; i < 10; i++) {
        err = type_table_add(&typetab, t[i]);
        if (err != NULL) {
            debug(err);
            err = NULL;
        }
    }
    // search test
    type search;
    type_init(&search);
    for (i = 9; i >= 0; i--) {
        search.type_name    = t[i].type_name;
        search.type_namelen = t[i].type_namelen;
        err = type_table_search(&typetab, &search);
        if (err != NULL) {
            debug(err);
            err = NULL;
        }
        else {
            printf("function name: %s\r\n", search.type_name);
        }
    }
    type_destroy(&search);

    printf("\r\n\r\n---Travel The Tree In Preorder---\r\n");
    type_table_preorder_travel(typetab.root);

    printf("\r\n\r\n---Travel The Tree In Inorder---\r\n");
    type_table_inorder_travel(typetab.root);

    printf("\r\n\r\n---Travel The Tree In Postorder---\r\n");
    type_table_postorder_travel(typetab.root);

    printf("\r\n\r\n---Count Every Branch's Black Node Amount---\r\n");
    int counted = 0;
    type_table_rec_black_count(typetab.root, counted);

    printf("\r\n\r\n---Calculate The Deepth Of The Tree---\r\n");
    printf("deepth: %d\r\n\r\n", type_table_deepth(typetab.root));

    type_table_destroy(&typetab);

    for (i = 0; i < 10; i++) {
        type_destroy(&t[i]);
    }
    debug("run over~");
    return 0;
}
