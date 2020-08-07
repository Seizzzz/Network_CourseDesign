#include <stdio.h>
#include <WinSock2.h>
#include <ws2tcpip.h> 
#pragma comment(lib, "ws2_32.lib")

typedef unsigned short uint16_t;
typedef short int16_t;
typedef int int32_t;
typedef unsigned char uint8_t;

const int BUF_SIZE = 512;

struct DNS_Header //DNS消息头部结构
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

struct DNS_Quest
{
    uint16_t QTYPE;
    uint16_t QCLASS;
};

int main(int argc, char* argv[])
{
    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
        printf("WSAStartup Failed!\n");
        return -1;
    }
    ///////////////////////////////////////////////

    SOCKET sockSrv = socket(AF_INET, SOCK_DGRAM, 0);
    SOCKET sockCli = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockSrv < 0 || sockCli < 0) {
        printf("Creating Sock Failed!\n");
        return -1;
    }

    SOCKADDR_IN addrSrv;
    const int addrSrvSize = sizeof(addrSrv);
    addrSrv.sin_addr.s_addr = INADDR_ANY;
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(53);
    bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
    ///////////////////////////////////////////////
    
    DNS_Header* ptrHeader = (DNS_Header*)buf;
    DNS_Quest* ptrQuest = (DNS_Quest*)(buf + sizeof(DNS_Header));

    while (true)
    {
        //接收报文
        int recvSize;
        recvSize = recvfrom(sockSrv, buf, BUF_SIZE, 0, (sockaddr*)&addrSrv, &addrSrvSize);
        if (sockSrv != INVALID_SOCKET) {
            printf("收到请求\n");
        }

        //报文内容 -dd
        for (int i = 0; i < recvSize; ++i) printf("%2.2x ", buf[i]);
        printf("\n");

        //报文地址内容 -d
        char* ptr = (char*)(buf + sizeof(DNS_Header));
        do
        {
            int cnt = *ptr;
            if (cnt == 0) break;
            for (; cnt > 0; --cnt) printf("%c", *(++ptr));
            printf(".");
            ++ptr;

        } while (true);
        printf("\n");

        //转发

    }


    ///////////////////////////////////////////////

    closesocket(sockSrv);
    WSACleanup();
}