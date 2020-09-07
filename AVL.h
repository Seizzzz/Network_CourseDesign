#pragma once

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

const int MAX_LENGTH_DOMAIN = 100; //域名最大字节长度
const int MAX_LENGTH_ADDR = 50; //地址最大字节长度

typedef struct T {
    int data;//节点数据
    int bal;//平衡因子
    char domain[MAX_LENGTH_DOMAIN];
    char addr_ip[MAX_LENGTH_ADDR];
    struct T* lchild, * rchild, * parent;//左孩子、右孩子、父亲节点
}*nodeptr, node;

void loadConfig();
char* search(char*);
void release();