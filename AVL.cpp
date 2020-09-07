#include "AVL.h"

BiTree T = nullptr;

void RotateRight(BiTree* parent, BiTree* T) {
    LNode* x = (*parent)->parent;//��¼���׽ڵ�
    LNode* SubL = (*parent)->lchild;//��¼���ڵ������
    LNode* SubLR = SubL->rchild;//���ڵ����ӵ��Һ���
    //parent������ΪSubLR
    (*parent)->lchild = SubLR;
    if (SubLR) {
        SubLR->parent = (*parent);
    }
    //SubL���Һ���Ϊparent
    SubL->rchild = (*parent);
    (*parent)->parent = SubL;
    /*--����x�ĺ��ӽڵ�--*/
    //��x������,��˵��parent���Ǹ��ڵ�
    if (!x) {
        (*T) = SubL;
        SubL->parent = NULL;
    }
    //�������ж�parent��x�����ӻ����Һ��ӣ�SubL�滻
    else {
        if (x->lchild == (*parent)) {
            x->lchild = SubL;
        }
        else {
            x->rchild = SubL;
        }
        SubL->parent = x;
    }
}
void RotateLeft(BiTree* parent, BiTree* T) {
    LNode* x = (*parent)->parent;
    LNode* SubR = (*parent)->rchild;
    LNode* SubRL = SubR->lchild;
    //parent���Һ���ΪSubRL
    (*parent)->rchild = SubRL;
    if (SubRL) {
        SubRL->parent = (*parent);
    }
    //SubR������Ϊparent
    SubR->lchild = (*parent);
    (*parent)->parent = SubR;
    //x�ĺ���ΪSubR
    if (!x) {
        (*T) = SubR;
        SubR->parent = NULL;
    }
    else {
        if (x->lchild == (*parent)) {
            x->lchild = SubR;
        }
        else {
            x->rchild = SubR;
        }
        SubR->parent = x;
    }
}
void RotateLR(BiTree* parent, BiTree* T) {
    //�ȶ�SubL������ת�仯���ٶ�parent������ת�仯
    RotateLeft(&((*parent)->lchild), T);
    RotateRight(parent, T);
}
void RotateRL(BiTree* parent, BiTree* T) {
    RotateRight(&((*parent)->rchild), T);
    RotateLeft(parent, T);
}
int Height(BiTree T) {
    int n, m;
    if (T == NULL) {
        return 0;
    }
    n = Height(T->rchild);
    m = Height(T->lchild);
    if (m > n) {
        return m + 1;
    }
    else {
        return n + 1;
    }
}
void Calbalance(BiTree T) {
    if (T == NULL) {
        return;
    }
    T->bal = Height(T->rchild) - Height(T->lchild);//�˴��������߶ȼ������߶�,������ƽ����ж���������
    //printf("%d %d\n",T->data,T->bal);
    Calbalance(T->rchild);
    Calbalance(T->lchild);
}
void InsertBalance(BiTree* x, BiTree* T) {

    LNode* parent = NULL;
    //x�ڵ��ǲ���ڵ�ĸ��׽ڵ�
    //������ڵ㲻���������
    if (*x) {
        parent = (*x)->parent;//�����丸�׽ڵ�
        while (parent) {
            //����3
            if (parent->bal < -1) {
                if (parent->lchild->bal == -1) {
                    RotateRight(&parent, T);
                }
                else {
                    RotateLR(&parent, T);
                }
                break;
            }
            if (parent->bal > 1) {
                if (parent->rchild->bal == 1) {
                    RotateLeft(&parent, T);
                }
                else {
                    RotateRL(&parent, T);
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
LNode* NewNode(char *domain,char *addr_ip) { //�����½ڵ�
    LNode* p = (LNode*)malloc(sizeof(LNode));
    p->bal = 0;
    //p->data = data;
    strcpy(p->domain, domain);
    strcpy(p->addr_ip, addr_ip);
    p->lchild = NULL;
    p->parent = NULL;
    p->rchild = NULL;
    return p;
}
void Insert(BiTree* T, char *domain,char *addr_ip) { //ƽ��������Ĳ�������
    LNode* parent = NULL;//��¼����ڵ�ĸ��׽ڵ�
    LNode* p = (*T);
    if ((*T) == NULL) {
        (*T) = NewNode(domain,addr_ip);
    }
    else {
        //�ҵ�����λ��
        while (p) {
            parent = p;//��¼���׽ڵ�
            if (strcmp(p->domain,domain)==0) {
                printf("fault :%s\n", p->domain);
                return;
            }
            else if (strcmp(p->domain, domain) == 1) {
                p = p->lchild;
            }
            else {
                p = p->rchild;
            }
        }
        //���½ڵ㲢����
        p = NewNode(domain,addr_ip);
        if (strcmp(parent->domain,domain)==1) {
            parent->lchild = p;
        }
        else {
            parent->rchild = p;
        }
        p->parent = parent;
    }
    //����ƽ�����Ӳ�ά��ƽ��
    Calbalance((*T));
    InsertBalance(&parent, T);
    Calbalance((*T));
}
void  OutTree(BiTree* T)  //n�ĳ�ֵΪ1
{
    if (*T)
    {
        //cout<<n<<':'<<t->data<<' ';

        OutTree(&(*T)->lchild);
       // printf("%d\n", (*T)->data);
        puts((*T)->domain);
        OutTree(&(*T)->rchild);
    }
}
LNode *FindIPaddr(BiTree *T, char *domain)
{
	if(*T)
	{
        int res = strcmp(domain, (*T)->domain);
        if (res == 0) return (*T);
        else if (res > 0) return FindIPaddr(&(*T)->rchild, domain);
        else return FindIPaddr(&(*T)->lchild, domain);
	}
    return NULL;
}

void loadConfig()
{
    char domain[100];
    char addr_ip[20];
    
    FILE* fp = fopen("dnsrelay.txt", "r");
    while (fscanf(fp, "%s", addr_ip) != EOF)
    {
        fscanf(fp, "%s", domain);
        Insert(&T, domain, addr_ip);
        //memset(domain, 0, sizeof(domain));
        //memset(addr_ip, 0, sizeof(addr_ip));
    }
}
char* search(char* domain)
{
    LNode* p = (LNode*)malloc(sizeof(LNode));
    p = FindIPaddr(&T, domain);
    //puts(p->addr_ip);
    if (p != NULL) return p->addr_ip;
    return NULL;
}