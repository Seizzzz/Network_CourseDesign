#pragma once

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct T {
    int data;//�ڵ�����
    int bal;//ƽ������
    char domain[100];
    char addr_ip[20];
    struct T* lchild, * rchild, * parent;//���ӡ��Һ��ӡ����׽ڵ�
}*BiTree, LNode;

void loadConfig();
char* search(char* domain);