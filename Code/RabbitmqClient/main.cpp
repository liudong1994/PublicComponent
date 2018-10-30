#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#include "RabbitmqClient.h"


int main()
{
    CRabbitmqClient objRabbitmq;

    vector<pair<string, int>> vecRabbitAddrs;
    vecRabbitAddrs.push_back(std::make_pair("192.168.240.141", 5672));
    vecRabbitAddrs.push_back(std::make_pair("192.168.240.142", 5672));
    std::string strUser = "admin";
    std::string strPasswd = "admin";

    if (0 != objRabbitmq.Connect(vecRabbitAddrs, strUser, strPasswd)) {
        printf("Rabbitmq Connect failed\n");
        return -1;
    }
    printf("Rabbitmq Connect success\n");

    
    std::string strExchange = "ExchangeTest";
    std::string strRoutekey = "routekeyTest";
    std::string strQueuename = "queueTest";

#if 0
    // 可选操作 Declare Exchange
    int iRet = objRabbitmq.ExchangeDeclare(strExchange, "direct");
    printf("Rabbitmq ExchangeDeclare Ret: %d\n", iRet);

    // 可选操作（接收） Declare Queue
    iRet = objRabbitmq.QueueDeclare(strQueuename);
    printf("Rabbitmq QueueDeclare Ret: %d\n", iRet);

    // 可选操作（接收） Queue Bind
    iRet = objRabbitmq.QueueBind(strQueuename, strExchange, strRoutekey);
    printf("Rabbitmq QueueBind Ret: %d\n", iRet);
#endif


    // Send Msg
#if 1
    while (1) {
        std::string strSendMsg = "rabbitmq send test msg";
        if (0 != objRabbitmq.Publish(strSendMsg, strExchange, strRoutekey)) {
            printf("Rabbitmq Publish failed, reconnect\n");

            while (0 != objRabbitmq.ReConnect()) {
                printf("Rabbitmq Reconnect failed\n");
                sleep(2);
            }
        }
        printf("Rabbitmq Publish success\n");
        sleep(10);
    }

#endif


    // Recv Msg
#if 0
    std::vector<std::string> vecRecvMsg;
    iRet = objRabbitmq.Consume(strQueuename, vecRecvMsg, 2);
    printf("Rabbitmq Consumer Ret: %d\n", iRet);

    for (size_t i=0; i<vecRecvMsg.size(); ++i) {
        printf("Consumer: %s\n", vecRecvMsg[i].c_str());
    }
#elif 0
    while(1) {
        // Recv Msg
        std::string strRecvMsg;
        uint64_t llAcktag = 0;
        int iRet = objRabbitmq.ConsumeNeedAck(strQueuename, strRecvMsg, llAcktag);
        printf("Rabbitmq Consumer Ret: %d  Tag: %lu  Msg: %s\n", iRet, llAcktag, strRecvMsg.c_str());
        objRabbitmq.ConsumeAck(llAcktag);
    }
#endif

    return 0;
}

