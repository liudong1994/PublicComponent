#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "MyRedis.h"


int StorageToRedis(MyRedis *pMyRedis, char *pBufStorage);

int main(int argc, char **argv)
{
    if(argc < 4)
    {
        printf("Usage: ./test RedisIP RedisPort RedisPasswd\n");
        printf("eg: ./test 172.17.9.204 6379 123456\n");

        printf("DataFormat(HSET): key filed value\n");
        printf("eg: key1 filed1 value1\n");
        return 0;
    }

    MyRedis *pMyRedis = new MyRedis;
    if(NULL == pMyRedis)
        return -1;

    // 连接Redis数据库
    printf("Redis Info: %s %s %s\n", argv[1], argv[2], argv[3]);
    pMyRedis->ConnectDB(argv[1], atoi(argv[2]), argv[3]);

    char bufStorage[1024];
    while(gets(bufStorage) != NULL && 0 != strcmp(bufStorage, "quit"))
    {
        printf("input: %s\n", bufStorage);
        if(0 != StorageToRedis(pMyRedis, bufStorage))
        {
            printf("Input Data Error OR Redis DisConnect\n");
            break;
        }
    }

    delete pMyRedis;
    pMyRedis = NULL;
    return 0;
}

int StorageToRedis(MyRedis *pMyRedis, char *pBufStorage)
{
    if(pMyRedis->IsConnect())
    {
        // 简单判断 提取HSET的 key filed value数据
        char *pKeyStart = pBufStorage;
        char *pKeyEnd = strstr(pBufStorage, " ");
        if(NULL == pKeyEnd)
            return -1;

        char *pFiledStart = pKeyEnd + 1; //skip " "
        char *pFiledEnd = strstr(pFiledStart, " ");
        if(NULL == pFiledEnd)
            return -1;

        char *pValueStart = pFiledEnd + 1;  //skip " "

        // 存储到Redis
        if(!pMyRedis->ExecSetCommand(REDIS_CMD_STRING, "HSET %S %S %s",
            (int)(pKeyEnd - pKeyStart), pKeyStart,          /*  Key  */
            (int)(pFiledEnd - pFiledStart), pFiledStart,    /* Filed */
                                           pValueStart))    /* Value */
        {
            return -1;
        }
    }
    else
    {
        printf("Redis ReConnectDB\n");
        sleep(1);
        if(!pMyRedis->ReConnectDB())
        {
            return -2;
        }
    }

    return 0;
}

