#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "tcp.h"

int main()
{
    CTcp *pTcp = new CTcp(9011, "192.168.240.141");
    pTcp->server_bind();
    pTcp->server_listen();

    std::string strResponse1 = "HTTP/1.1 200 ";
    std::string strResponse2 = "OK\r\nContent-Type: application/json; charset=utf-8\r\nContent-Length: ";
    std::string strResponse3 = "78\r\n\r\n{\"errNo\":0,\"errStr\":\"\",\"data\":{\"skuIds\":{\"1\":0,\"2\":0},\"actIds\":{\"1\":1,\"2\":1}}}";

    // accept and recvstring
    char buf[8192]={0};
	while(1) {
        CTcp *pTcpClient = pTcp->server_accept();
        if (pTcpClient) {
            pTcpClient->tcp_read(buf, 8192);
            printf("tcp client recv string:\n%s\n", buf);

            pTcpClient->tcp_writeall(strResponse1.c_str(), strResponse1.size());
            printf("send response string:\n%s\n", strResponse1.c_str());
            sleep(1);

            pTcpClient->tcp_writeall(strResponse2.c_str(), strResponse2.size());
            printf("send response string:\n%s\n", strResponse2.c_str());
            sleep(1);

            pTcpClient->tcp_writeall(strResponse3.c_str(), strResponse3.size());
            printf("send response string:\n%s\n", strResponse3.c_str());
            sleep(1);

            pTcpClient->tcp_close();   
        }
	}

    return 0;
}

