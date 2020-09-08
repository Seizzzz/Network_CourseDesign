#include <stdio.h>
#include <WinSock2.h>
#include <ws2tcpip.h> 
#include "AVL.h"
#include "transid.h"
#pragma comment(lib, "ws2_32.lib")
#pragma pack(1) //阻止字节对齐

typedef unsigned short uint16_t;
typedef short int16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;

const int BUF_SIZE = 512;
char addrDNS[MAX_LENGTH_ADDR] = "114.114.114.114";
char addrFile[50];
int infoLevel = 0;

struct dnsHeader //DNS头
{
    uint16_t ID;
    uint16_t TAG;
        /*uint16_t QR : 1;
        uint16_t Opcode : 4;
        uint16_t AA : 1;
        uint16_t TC : 1;
        uint16_t RD : 1;
        uint16_t RA : 1;
        uint16_t Z : 3;
        uint16_t RCODE : 4;*/
    uint16_t QDCOUNT; //问题数
    uint16_t ANCOUNT; //回答数
    uint16_t NSCOUNT; //授权数
    uint16_t ARCOUNT; //附加记录数
};

struct dnsRR //资源记录
{
    uint16_t NAME;
    uint16_t TYPE;
    uint16_t CLASS;
    uint32_t TTL;
    uint16_t RDLENGTH;
    uint32_t RDATA; //后可变长
};
/*
    const char* a[] = { "china","germany","america" };
    construct(0x0, 0, a, 3);
*/

int isIpv4(const char* a)
{
    if (strchr(a, ':') == NULL) return 1; //若包含':'则为IPv6地址
    return 0;
}

int build(char* buf, int size, const char* a[], uint16_t num)
{
    dnsHeader* hdr = (dnsHeader*)buf;
    if (strcmp(a[0], "0.0.0.0") == 0 || num == 0 || a[0] == NULL) { //拦截
        hdr->TAG = htons(0x8183); //0x8183 No such name
        return size;
    }
    
    dnsRR* rr = (dnsRR*)(buf + size);
    hdr->ANCOUNT = htons(num);
    hdr->TAG = htons(0x8180); //0x8180 No error
    for (int i = 0; i < num; ++i)
    {
        if (isIpv4(a[i])) { //ipv4
            rr->NAME = htons(0xc00c); //CNAME
            rr->TYPE = htons(0x0001); //A记录(ipv4)
            rr->CLASS = htons(0x0001); //IN(Internet类别)
            rr->TTL = htonl(0x3c); 
            rr->RDLENGTH = htons(4); //rdata长度
            inet_pton(AF_INET, a[i], &rr->RDATA);
            rr = (dnsRR*)((int)rr + 12 + 4); //rr指针 + rr长度 + ipv4长度 = 下一个rr的起始位置
        }
        else //ipv6
        {
            rr->NAME = htons(0xc00c);
            rr->TYPE = htons(0x001c); //AAAA记录(ipv6)
            rr->CLASS = htons(0x0001);
            rr->TTL = htonl(0x3c);
            rr->RDLENGTH = htons(16);
            inet_pton(AF_INET6, a[i], &rr->RDATA);
            rr = (dnsRR*)((int)rr + 12 + 16); //rr指针 + rr长度 + ipv6长度 = 下一个rr的起始位置
        }
    }

    return (int)rr - (int)hdr;
}

void toDomain(char* src, char* dst) //将长度+字符形式转换为通常域名格式
{
    char* ptr = src + sizeof(dnsHeader);
    do
    {
        int cnt = *ptr; //字符数
        if (cnt == 0) break;
        for (; cnt > 0; --cnt) dst += sprintf(dst, "%c", *(++ptr)); //写字符
        dst += sprintf(dst, ".");
        ++ptr;
    } while (true);
    
    --dst; //格式化，去除最后的'.'
    *dst = '\0';
}

void printBuf(char* buf, const int conSize)
{
    if (infoLevel >= 2) //报文原始内容 -dd
    {
        for (int i = 0; i < conSize; ++i) {
            printf("%2.2x ", (unsigned char)buf[i]);
            if (i % 16 == 15) printf("\n");
        }
        printf("\n");
    }

    //报文翻译内容 -d
    if (infoLevel >= 1)
    {
        printf("Domain: ");
        char* domain = (char*)malloc(MAX_LENGTH_DOMAIN);
        toDomain(buf, domain);
        printf("%s\n", domain);
        free(domain);
    }

    return;
}

int requestDNS(char* buf, int recvSize)
{
    SOCKET sockCli = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockCli < 0) {
        printf("Creating Sock(requestDNS) Failed!\n");
        exit(-1);
    }

    SOCKADDR_IN addrCli;
    int addrCliSize = sizeof(addrCli);
    addrCli.sin_addr.s_addr = inet_addr(addrDNS); //向默认DNS服务器查询
    addrCli.sin_family = AF_INET;
    addrCli.sin_port = htons(53);
    bind(sockCli, (SOCKADDR*)&addrCli, sizeof(SOCKADDR));

    //转发请求报文
    printf("Relay Request to %s:%d\n", inet_ntoa(addrCli.sin_addr), ntohs(addrCli.sin_port));
    sendto(sockCli, buf, recvSize, 0, (SOCKADDR*)&addrCli, sizeof(addrCli));

    //接收响应报文
    recvSize = recvfrom(sockCli, buf, BUF_SIZE, 0, (SOCKADDR*)&addrCli, &addrCliSize);

    if (sockCli != INVALID_SOCKET) {
        printf("Recv Response from %s:%d\n", inet_ntoa(addrCli.sin_addr), ntohs(addrCli.sin_port));
    }
    printBuf(buf, recvSize);

    closesocket(sockCli);
    return recvSize;
}

void getRequest(char* buf)
{
    int recvSize; //缓冲区有效信息大小

    SOCKET sockSrv = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockSrv < 0) {
        printf("Creating Socket(getRequest) Failed!\n");
        exit(-1);
    }

    SOCKADDR_IN addrSrv;
    int addrSrvSize = sizeof(addrSrv);
    addrSrv.sin_addr.s_addr = INADDR_ANY; //接收所有端口的请求信息
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(53);
    bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));

    //接收请求报文
    recvSize = recvfrom(sockSrv, buf, BUF_SIZE, 0, (SOCKADDR*)&addrSrv, &addrSrvSize);
    if (sockSrv != INVALID_SOCKET) {
        printf("Recv Request from %s:%d\n", inet_ntoa(addrSrv.sin_addr), ntohs(addrSrv.sin_port));
    }
    printBuf(buf, recvSize);

    //ID旧转新
    int oldID = ((dnsHeader*)buf)->ID;
    int newID = saveID(oldID);
    ((dnsHeader*)buf)->ID = newID; //存储旧ID
    if (infoLevel >= 2) printf("(ID:%d->%d)\n", oldID, newID);

    //查询
    char* domain = (char*)malloc(MAX_LENGTH_DOMAIN);
    toDomain(buf, domain);
    const char* ret = search(domain);
    const char* res[] = { ret };
    if (ret != NULL) //从配置文件中查询到，构造响应报文
    {
        if (infoLevel >= 2) printf("Found record in file!\n");
        recvSize = build(buf, recvSize, res, 1);
    }
    else //从默认DNS服务器获取
    {
        if (infoLevel >= 2) printf("Requesting DNS server!\n");
        recvSize = requestDNS(buf, recvSize);
    }
    free(domain);

    //ID新转旧
    newID = ((dnsHeader*)buf)->ID;
    oldID = loadID(((dnsHeader*)buf)->ID);
    ((dnsHeader*)buf)->ID = oldID; //取回旧ID
    if (infoLevel >= 2) printf("(ID:%d->%d)\n", newID, oldID);

    //回复响应报文
    printf("Relay Response to %s:%d\n", inet_ntoa(addrSrv.sin_addr), ntohs(addrSrv.sin_port));
    sendto(sockSrv, buf, recvSize, 0, (SOCKADDR*)&addrSrv, sizeof(addrSrv));

    closesocket(sockSrv);
    return;
}

int main(int argc, char* argv[])
{
    if (argc > 1) //处理运行参数
    {
        strcpy(addrFile, "./dnsrelay.txt");
        infoLevel = 0;
        for (int i = 1; i < argc; ++i)
        {
            if (argv[i][0] == '-')
            {
                if (strcmp(argv[i], "-d") == 0) infoLevel = 1;
                else if (strcmp(argv[i], "-dd") == 0) infoLevel = 2;
                else
                {
                    printf("Unknown Command!\n");
                    exit(-1);
                }
            }
            else if (isdigit(argv[i][0])) strcpy(addrDNS, argv[i]);
            else strcpy(addrFile, argv[i]);
        }
    }

    loadConfig(addrFile); //从文件加载配置
    initTrans(); //初始化ID转换

    WSADATA wsaData; //加载Winsock库
    if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
        printf("WSAStartup Failed!\n");
        return -1;
    }

    char buf[BUF_SIZE]; //缓冲区
    memset(buf, 0, BUF_SIZE);

    while (1) //循环接受请求报文
    {
        getRequest(buf);
    }

    WSACleanup(); //释放Winsock库
    release(); //释放树
    releaseID(); //释放ID转换
}