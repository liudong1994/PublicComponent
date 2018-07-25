#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "BlockQueue.h"

using std::string;

#define LOOP_NUMS 100

struct BlockTest {
    int iVersion;
    char *pName;

    BlockTest(): iVersion(0), pName(NULL) {

    }
};


void *TH_Send(void *queue);
void *TH_Recv(void *queue);

int main()
{
    CBlockQueue<BlockTest> queue;
    
    pthread_t thSendThread;
    int iRet = pthread_create(&thSendThread, NULL, &TH_Send, &queue);
    if (iRet != 0) {
        printf("start send thread failed\n");
        return -1;
    }

    pthread_t thRecvThread;
    iRet = pthread_create(&thRecvThread, NULL, &TH_Recv, &queue);
    if (iRet != 0) {
        printf("start recv thread failed\n");
        return -1;
    }


    pthread_join(thSendThread, NULL);
    pthread_join(thRecvThread, NULL);

    return 0;
}

void *TH_Send(void *pContext) 
{
    CBlockQueue<BlockTest> *queue = (CBlockQueue<BlockTest> *)pContext;

    for(int i=0; i<LOOP_NUMS; ++i) {
        BlockTest block;
        block.iVersion = i;
        block.pName = (char *)malloc(20);
        snprintf(block.pName, 20, "Block_%d", i);

        printf("TH_Send Name: %s, Version: %d\n", block.pName, block.iVersion);
        queue->put(block);
    }

    return NULL;
}

void *TH_Recv(void *pContext)
{
    CBlockQueue<BlockTest> *queue = (CBlockQueue<BlockTest> *)pContext;

    for(int i=0; i<LOOP_NUMS; ++i) {
        BlockTest block(queue->take());
        printf("TH_Recv Name: %s, Version: %d\n", block.pName, block.iVersion);
        free(block.pName);
    }

    return NULL;
}

