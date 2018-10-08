#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#include "RabbitmqClient.h"


int MsgCallback(const std::string &strMsg);

int main()
{
    CRabbitmqClient objRabbitmq;

    std::string strIP = "192.168.240.121";
    int iPort = 8856;
    std::string strUser = "rabbitadmin";
    std::string strPasswd = "123321";


    int iRet = objRabbitmq.Connect(strIP, iPort, strUser, strPasswd);
    printf("Rabbitmq Connect Ret: %d\n", iRet);

    
    std::string strExchange = "ExchangeTest";
    std::string strRoutekey = "routekeyTest6";
    std::string strQueuename = "queueTest6";

#if 0
    // 可选操作 Declare Exchange
    iRet = objRabbitmq.ExchangeDeclare(strExchange, "direct");
    printf("Rabbitmq ExchangeDeclare Ret: %d\n", iRet);

    // 可选操作（接收） Declare Queue
    iRet = objRabbitmq.QueueDeclare(strQueuename);
    printf("Rabbitmq QueueDeclare Ret: %d\n", iRet);

    // 可选操作（接收） Queue Bind
    iRet = objRabbitmq.QueueBind(strQueuename, strExchange, strRoutekey);
    printf("Rabbitmq QueueBind Ret: %d\n", iRet);

    // Send Msg
    std::string strSendMsg1 = "rabbitmq send test msg1";
    std::string strSendMsg2 = "rabbitmq send test msg2";
    iRet = objRabbitmq.Publish(strSendMsg1, strExchange, strRoutekey);
    printf("Rabbitmq Publish 1 Ret: %d\n", iRet);
    iRet = objRabbitmq.Publish(strSendMsg2, strExchange, strRoutekey);
    printf("Rabbitmq Publish 2 Ret: %d\n", iRet);
#endif


#if 0
    // Recv Msg
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
        iRet = objRabbitmq.ConsumeNeedAck(strQueuename, strRecvMsg, llAcktag);
        printf("Rabbitmq Consumer Ret: %d  Tag: %lu  Msg: %s\n", iRet, llAcktag, strRecvMsg.c_str());
        iRet = objRabbitmq.ConsumeAck(iRet, llAcktag);
    }
#else
    // Recv Msg
    objRabbitmq.ConsumeThread(strQueuename, MsgCallback);
#endif

    //sleep(20);

    objRabbitmq.SetConsumeThreadRunStatus(false);
    objRabbitmq.Disconnect();
    return 0;
}

int MsgCallback(const std::string &strMsg) {
    return 0;
}

