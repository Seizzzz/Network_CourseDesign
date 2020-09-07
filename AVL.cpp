#include "AVL.h"

nodeptr T = NULL;

void rotateRight(node* parent, node* T) {
    node* x = parent->parent;//��¼���׽ڵ�
    node* SubL = parent->lchild;//��¼���ڵ������
    node* SubLR = SubL->rchild;//���ڵ����ӵ��Һ���
    //parent������ΪSubLR
    parent->lchild = SubLR;
    if (SubLR) {
        SubLR->parent = parent;
    }
    //SubL���Һ���Ϊparent
    SubL->rchild = parent;
    parent->parent = SubL;
    /*--����x�ĺ��ӽڵ�--*/
    //��x������,��˵��parent���Ǹ��ڵ�
    if (!x) {
        T = SubL;
        SubL->parent = NULL;
    }
    //�������ж�parent��x�����ӻ����Һ��ӣ�SubL�滻
    else {
        if (x->lchild == parent) {
            x->lchild = SubL;
        }
        else {
            x->rchild = SubL;
        }
        SubL->parent = x;
    }
}

void rotateLeft(node* parent, node* T) {
    node* x = parent->parent;
    node* SubR = parent->rchild;
    node* SubRL = SubR->lchild;
    //parent���Һ���ΪSubRL
    parent->rchild = SubRL;
    if (SubRL) {
        SubRL->parent = parent;
    }
    //SubR������Ϊparent
    SubR->lchild = parent;
    parent->parent = SubR;
    //x�ĺ���ΪSubR
    if (!x) {
        T = SubR;
        SubR->parent = NULL;
    }
    else {
        if (x->lchild == parent) {
            x->lchild = SubR;
        }
        else {
            x->rchild = SubR;
        }
        SubR->parent = x;
    }
}

void rotateLR(node* parent, node* T) {
    //�ȶ�SubL������ת�仯���ٶ�parent������ת�仯
    rotateLeft(parent->lchild, T);
    rotateRight(parent, T);
}

void rotateRL(node* parent, node* T) {
    rotateRight(parent->rchild, T);
    rotateLeft(parent, T);
}

int height(nodeptr T) {
    int n, m;
    if (T == NULL) {
        return 0;
    }
    n = height(T->rchild);
    m = height(T->lchild);
    if (m > n) {
        return m + 1;
    }
    else {
        return n + 1;
    }
}

void rebalance(nodeptr T) {
    if (T == NULL) {
        return;
    }
    T->bal = height(T->rchild) - height(T->lchild);//�˴��������߶ȼ������߶�,������ƽ����ж���������
    //printf("%d %d\n",T->data,T->bal);
    rebalance(T->rchild);
    rebalance(T->lchild);
}

void insertBalance(node* x, node* T) {
    node* parent = NULL;
    //x�ڵ��ǲ���ڵ�ĸ��׽ڵ�
    //������ڵ㲻���������
    if (x) {
        parent = x->parent;//�����丸�׽ڵ�
        while (parent) {
            //����3
            if (parent->bal < -1) {
                if (parent->lchild->bal == -1) {
                    rotateRight(parent, T);
                }
                else {
                    rotateLR(parent, T);
                }
                break;
            }
            if (parent->bal > 1) {
                if (parent->rchild->bal == 1) {
                    rotateLeft(parent, T);
                }
                else {
                    rotateRL(parent, T);
                }
                break;
            }
            //����2
            else if (parent->bal == 0) {
                break;
            }
            //����1
            parent = parent->parent;
        }
    }
}

node* newNode(char *domain,char *addr_ip) { //�����½ڵ�
    node* p = (node*)malloc(sizeof(node));
    p->bal = 0;
    //p->data = data;
    strcpy(p->domain, domain);
    strcpy(p->addr_ip, addr_ip);
    p->lchild = NULL;
    p->parent = NULL;
    p->rchild = NULL;
    return p;
}

void insert(node* now, char *domain,char *addr_ip) { //ƽ��������Ĳ�������
    node* parent = NULL;//��¼����ڵ�ĸ��׽ڵ�
    node* p = now;

    if (now == NULL) {
        now = newNode(domain, addr_ip);
        if (T == NULL) T = now;
    }
    else {
        //�ҵ�����λ��
        while (p) {
            parent = p;//��¼���׽ڵ�
            if (strcmp(p->domain,domain) == 0) {
                printf("fault :%s\n", p->domain);
                return;
            }
            else if (strcmp(p->domain, domain) > 0) {
                p = p->lchild;
            }
            else {
                p = p->rchild;
            }
        }
        //���½ڵ㲢����
        p = newNode(domain,addr_ip);
        if (strcmp(parent->domain,domain) > 0) {
            parent->lchild = p;
        }
        else {
            parent->rchild = p;
        }
        p->parent = parent;
    }
    //����ƽ�����Ӳ�ά��ƽ��
    rebalance(now);
    insertBalance(parent, now);
    rebalance(now);
}

node* findIPaddr(node* T, char *domain)
{
	if(T)
	{
        int res = strcmp(domain, T->domain);
        if (res == 0) return T;
        else if (res > 0) return findIPaddr(T->rchild, domain);
        else return findIPaddr(T->lchild, domain);
	}
    return NULL;
}

void loadConfig()
{
    char domain[MAX_LENGTH_DOMAIN];
    char addr_ip[MAX_LENGTH_ADDR];

    FILE* fp = fopen("dnsrelay.txt", "r");
    while (fscanf(fp, "%s", addr_ip) != EOF)
    {
        fscanf(fp, "%s", domain);
        insert(T, domain, addr_ip);
        while (T->parent != NULL) T = T->parent;
    }

    //printf("fuck");
}

char* search(char* domain)
{
    node* p = findIPaddr(T, domain);
    if (p != NULL) return p->addr_ip;
    return NULL;
}

void recRelease(node* p)
{
    if (p->lchild)
    {
        recRelease(p->lchild);
        free(p->lchild);
    }
    if (p->rchild)
    {
        recRelease(p->rchild);
        free(p->rchild);
    }
    free(p);
}

void release()
{
    recRelease(T);
}