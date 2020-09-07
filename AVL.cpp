#include "AVL.h"

nodeptr T = NULL;

void rotateRight(node* parent, node* T) {
    node* x = parent->parent;//记录父亲节点
    node* SubL = parent->lchild;//记录根节点的左孩子
    node* SubLR = SubL->rchild;//根节点左孩子的右孩子
    //parent的左孩子为SubLR
    parent->lchild = SubLR;
    if (SubLR) {
        SubLR->parent = parent;
    }
    //SubL的右孩子为parent
    SubL->rchild = parent;
    parent->parent = SubL;
    /*--更新x的孩子节点--*/
    //若x不存在,则说明parent即是根节点
    if (!x) {
        T = SubL;
        SubL->parent = NULL;
    }
    //存在则判断parent是x的左孩子还是右孩子，SubL替换
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
    //parent的右孩子为SubRL
    parent->rchild = SubRL;
    if (SubRL) {
        SubRL->parent = parent;
    }
    //SubR的左孩子为parent
    SubR->lchild = parent;
    parent->parent = SubR;
    //x的孩子为SubR
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
    //先对SubL进行左转变化、再对parent进行右转变化
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
    T->bal = height(T->rchild) - height(T->lchild);//此处是右树高度减左树高度,在重新平衡的判定中起到作用
    //printf("%d %d\n",T->data,T->bal);
    rebalance(T->rchild);
    rebalance(T->lchild);
}

void insertBalance(node* x, node* T) {
    node* parent = NULL;
    //x节点是插入节点的父亲节点
    //若插入节点不是树根结点
    if (x) {
        parent = x->parent;//考察其父亲节点
        while (parent) {
            //情形3
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
            //情形2
            else if (parent->bal == 0) {
                break;
            }
            //情形1
            parent = parent->parent;
        }
    }
}

node* newNode(char *domain,char *addr_ip) { //创建新节点
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

void insert(node* now, char *domain,char *addr_ip) { //平衡二叉树的插入运算
    node* parent = NULL;//记录插入节点的父亲节点
    node* p = now;

    if (now == NULL) {
        now = newNode(domain, addr_ip);
        if (T == NULL) T = now;
    }
    else {
        //找到插入位置
        while (p) {
            parent = p;//记录父亲节点
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
        //创新节点并插入
        p = newNode(domain,addr_ip);
        if (strcmp(parent->domain,domain) > 0) {
            parent->lchild = p;
        }
        else {
            parent->rchild = p;
        }
        p->parent = parent;
    }
    //更新平衡因子并维护平衡
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