#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <thread>

#include "RabbitmqClient.h"


int main()
{
    vector<pair<string, int>> vecRabbitAddrs;
    vecRabbitAddrs.push_back(std::make_pair("192.168.240.121", 8856));
    vecRabbitAddrs.push_back(std::make_pair("192.168.240.224", 8856));
    vecRabbitAddrs.push_back(std::make_pair("192.168.240.226", 8856));
    std::string strUser = "rabbitadmin";
    std::string strPasswd = "123321";

    // init rabbitmq
    if (0 != CRabbitmqClient::Init(1, vecRabbitAddrs, strUser, strPasswd, 3, 10)) {
        fprintf(stderr, "AMQP_TYPE_FULL connect failed\n");
        return -1;
    }

    // get rabbimq
    shared_ptr<CRabbitmqClient> pAMQPClient;
    CRabbitmqClient::getInstance(1, pAMQPClient);

    

    std::string strExchange = "ExchangeTest";
    std::string strRoutekey = "routekeyTest";
    std::string strQueuename = "queueTest";

#if 1
    // 可选操作 Declare Exchange
    int iRet = pAMQPClient->ExchangeDeclare(strExchange, "direct");
    printf("Rabbitmq ExchangeDeclare Ret: %d\n", iRet);

    // 可选操作（接收） Declare Queue
    iRet = pAMQPClient->QueueDeclare(strQueuename);
    printf("Rabbitmq QueueDeclare Ret: %d\n", iRet);

    // 可选操作（接收） Queue Bind
    iRet = pAMQPClient->QueueBind(strQueuename, strExchange, strRoutekey);
    printf("Rabbitmq QueueBind Ret: %d\n", iRet);
#endif

    while (1) {
        pAMQPClient->SendHeartbeats();
        sleep(8);
    }

    string strInput("");

    // Send Msg
#if 1
    while (1) {
        fprintf(stderr, "Input 'y' send one message\n");
        cin >> strInput;
        if (strInput == "y") {
            std::string strSendMsg = "rabbitmq send test msg";
            while (0 != pAMQPClient->Publish(strSendMsg, strExchange, strRoutekey)) {
                printf("Rabbitmq Publish failed, reconnect\n");

                while (0 != pAMQPClient->ReConnect()) {
                    printf("Rabbitmq Reconnect failed\n");
                    sleep(2);
                }
            }

            printf("Rabbitmq Publish success\n");
            continue;
        }

        break;
    }
#endif


    // Recv Msg
#if 0
    std::vector<std::string> vecRecvMsg;
    iRet = pAMQPClient->Consume(strQueuename, vecRecvMsg, 2);
    printf("Rabbitmq Consumer Ret: %d\n", iRet);

    for (size_t i=0; i<vecRecvMsg.size(); ++i) {
        printf("Consumer: %s\n", vecRecvMsg[i].c_str());
    }
#elif 1
    while(1) {
        fprintf(stderr, "Input 'y' recv one message\n");
        cin >> strInput;
        if (strInput == "y") {
            // Recv Msg
            std::string strRecvMsg;
            uint64_t llAcktag = 0;
            while (0 != pAMQPClient->ConsumeNeedAck(strQueuename, strRecvMsg, llAcktag)) {
                printf("Rabbitmq Consumer failed, reconnect\n");

                while (0 != pAMQPClient->ReConnect()) {
                    printf("Rabbitmq Reconnect failed\n");
                    sleep(2);
                }
            }
            printf("Rabbitmq Consumer Tag: %lu  Msg: %s\n", llAcktag, strRecvMsg.c_str());
            pAMQPClient->ConsumeAck(llAcktag);
            continue;
        }

        break;
    }
#endif

    return 0;
}

