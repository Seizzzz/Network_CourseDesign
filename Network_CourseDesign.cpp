#include <stdio.h>
#include <WinSock2.h>
#include <ws2tcpip.h> 
#pragma comment(lib, "ws2_32.lib")

typedef unsigned short uint16_t;
typedef short int16_t;
typedef int int32_t;
typedef unsigned char uint8_t;

const int BUF_SIZE = 512;
char addrDNS[20] = "114.114.114.114";
char addrFile[50];
int infoLevel = 0;

struct dnsHeader //DNS消息头部结构
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
    uint16_t QDCOUNT;
    uint16_t ANCOUNT;
    uint16_t NSCOUNT;
    uint16_t ARCOUNT;
};

void printBuf(char* buf, const int conSize)
{
    if (infoLevel >= 2) //报文原始内容 -dd
    {
        for (int i = 0; i < conSize; ++i) printf("%2.2x ", (unsigned char)buf[i]);
        printf("\n");
    }

    //报文翻译内容 -d
    if (infoLevel >= 1)
    {
        char* ptr = buf + sizeof(dnsHeader);
        do
        {
            int cnt = *ptr;
            if (cnt == 0) break;
            for (; cnt > 0; --cnt) printf("%c", *(++ptr));
            printf(".");
            ++ptr;

        } while (true);
        printf("\n");
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
    addrCli.sin_addr.s_addr = inet_addr(addrDNS);
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
    int recvSize;

    SOCKET sockSrv = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockSrv < 0) {
        printf("Creating Socket(getRequest) Failed!\n");
        exit(-1);
    }

    SOCKADDR_IN addrSrv;
    int addrSrvSize = sizeof(addrSrv);
    addrSrv.sin_addr.s_addr = INADDR_ANY;
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(53);
    bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));

    //接收请求报文
    recvSize = recvfrom(sockSrv, buf, BUF_SIZE, 0, (SOCKADDR*)&addrSrv, &addrSrvSize);
    if (sockSrv != INVALID_SOCKET) {
        printf("Recv Request from %s:%d\n", inet_ntoa(addrSrv.sin_addr), ntohs(addrSrv.sin_port));
    }
    printBuf(buf, recvSize);

    //查询
    recvSize = requestDNS(buf, recvSize);

    //回复响应报文
    printf("Relay Response to %s:%d\n", inet_ntoa(addrSrv.sin_addr), ntohs(addrSrv.sin_port));
    sendto(sockSrv, buf, recvSize, 0, (SOCKADDR*)&addrSrv, sizeof(addrSrv));

    closesocket(sockSrv);
    return;
}

int main(int argc, char* argv[])
{
    if (argc > 1)
    {
        strcpy(addrFile, argv[0]);
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
            else if (isalpha(argv[i][0])) strcpy(addrFile, argv[i]);
        }
    }

    ///////////////////////////////////////////////

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
        printf("WSAStartup Failed!\n");
        return -1;
    }

    ///////////////////////////////////////////////

    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);

    while (true)
    {
        getRequest(buf);
    }

    ///////////////////////////////////////////////

    WSACleanup();
}