#include "AVL.h"

BiTree T = nullptr;

void RotateRight(BiTree* parent, BiTree* T) {
    LNode* x = (*parent)->parent;//记录父亲节点
    LNode* SubL = (*parent)->lchild;//记录根节点的左孩子
    LNode* SubLR = SubL->rchild;//根节点左孩子的右孩子
    //parent的左孩子为SubLR
    (*parent)->lchild = SubLR;
    if (SubLR) {
        SubLR->parent = (*parent);
    }
    //SubL的右孩子为parent
    SubL->rchild = (*parent);
    (*parent)->parent = SubL;
    /*--更新x的孩子节点--*/
    //若x不存在,则说明parent即是根节点
    if (!x) {
        (*T) = SubL;
        SubL->parent = NULL;
    }
    //存在则判断parent是x的左孩子还是右孩子，SubL替换
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
    //parent的右孩子为SubRL
    (*parent)->rchild = SubRL;
    if (SubRL) {
        SubRL->parent = (*parent);
    }
    //SubR的左孩子为parent
    SubR->lchild = (*parent);
    (*parent)->parent = SubR;
    //x的孩子为SubR
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
    //先对SubL进行左转变化、再对parent进行右转变化
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
    T->bal = Height(T->rchild) - Height(T->lchild);//此处是右树高度减左树高度,在重新平衡的判定中起到作用
    //printf("%d %d\n",T->data,T->bal);
    Calbalance(T->rchild);
    Calbalance(T->lchild);
}
void InsertBalance(BiTree* x, BiTree* T) {

    LNode* parent = NULL;
    //x节点是插入节点的父亲节点
    //若插入节点不是树根结点
    if (*x) {
        parent = (*x)->parent;//考察其父亲节点
        while (parent) {
            //情形3
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
            //情形2
            else if (parent->bal == 0) {
                break;
            }
            //情形1
            parent = parent->parent;
        }
    }
}
LNode* NewNode(char *domain,char *addr_ip) { //创建新节点
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
void Insert(BiTree* T, char *domain,char *addr_ip) { //平衡二叉树的插入运算
    LNode* parent = NULL;//记录插入节点的父亲节点
    LNode* p = (*T);
    if ((*T) == NULL) {
        (*T) = NewNode(domain,addr_ip);
    }
    else {
        //找到插入位置
        while (p) {
            parent = p;//记录父亲节点
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
        //创新节点并插入
        p = NewNode(domain,addr_ip);
        if (strcmp(parent->domain,domain)==1) {
            parent->lchild = p;
        }
        else {
            parent->rchild = p;
        }
        p->parent = parent;
    }
    //更新平衡因子并维护平衡
    Calbalance((*T));
    InsertBalance(&parent, T);
    Calbalance((*T));
}
void  OutTree(BiTree* T)  //n的初值为1
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