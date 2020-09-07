#pragma once

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct T {
    int data;//节点数据
    int bal;//平衡因子
    char domain[100];
    char addr_ip[20];
    struct T* lchild, * rchild, * parent;//左孩子、右孩子、父亲节点
}*BiTree, LNode;

void loadConfig();
char* search(char* domain);