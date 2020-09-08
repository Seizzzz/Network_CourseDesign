#pragma once

#include <stdlib.h>

struct nodeID
{
	int from;
	int to;
	nodeID* next;
};

void initTrans();

int saveID(int id); //返回转换后的id  传入原本的id
int loadID(int id); //返回原本的id  传入转换后的id
void releaseID();