#include "transid.h"

nodeID* hdr;
nodeID* tail;
int avaliableID = 0;

void initTrans()
{
	hdr = (nodeID*)malloc(sizeof(nodeID));
	tail = (nodeID*)malloc(sizeof(nodeID));
	hdr->next = tail;
	tail->next = NULL;
}

void insert(nodeID* p)
{
	nodeID* first = hdr->next;
	hdr->next = p;
	p->next = first;
}

int saveID(int id)
{
	nodeID* tmp = (nodeID*)malloc(sizeof(nodeID));
	tmp->from = id;
	tmp->to = ++avaliableID;

	insert(tmp);

	return tmp->to;
}

void remove(nodeID* pre, nodeID* p)
{
	pre->next = p->next;
	free(p);
}

int loadID(int id)
{
	nodeID* pre = hdr;
	nodeID* now = hdr->next;
	int ret = -1;

	while (now->next != NULL)
	{
		if (now->to == id)
		{
			ret = now->from;
			remove(pre, now);
			break;
		}

		pre = now;
		now = now->next;
	}

	return ret;
}

void releaseID()
{
	nodeID* p = hdr;
	nodeID* np = hdr->next;

	while (np != NULL)
	{
		free(p);
		p = np;
		np = np->next;
	}
}