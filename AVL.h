#pragma once

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

const int MAX_LENGTH_DOMAIN = 100; //��������ֽڳ���
const int MAX_LENGTH_ADDR = 50; //��ַ����ֽڳ���

typedef struct T {
    int data;//�ڵ�����
    int bal;//ƽ������
    char domain[MAX_LENGTH_DOMAIN];
    char addr_ip[MAX_LENGTH_ADDR];
    struct T* lchild, * rchild, * parent;//���ӡ��Һ��ӡ����׽ڵ�
}*nodeptr, node;

void loadConfig();
char* search(char*);
void release();