#pragma once

#include <stdlib.h>

struct nodeID
{
	int from;
	int to;
	nodeID* next;
};

void initTrans();

int saveID(int id); //����ת�����id  ����ԭ����id
int loadID(int id); //����ԭ����id  ����ת�����id
void releaseID();