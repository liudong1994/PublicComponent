#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <thread>     // C++11

#include "BlockQueue.h"

using std::string;

#define LOOP_NUMS 100

struct BlockTest {
    int iVersion;
    string strName;

    BlockTest(): iVersion(0) {

    }
};


void *TH_Send(void *queue);
void *TH_Recv(void *queue);

int main()
{
    CBlockQueue<BlockTest> queue;
    
    std::thread thSend(TH_Send, &queue);
    std::thread thRecv(TH_Recv, &queue);

    thSend.join();
    thRecv.join();

    return 0;
}

void *TH_Send(void *pContext) 
{
    CBlockQueue<std::shared_ptr<BlockTest>> *queue = (CBlockQueue<std::shared_ptr<BlockTest>> *)pContext;

    for(int i=0; i<LOOP_NUMS; ++i) {
        std::shared_ptr<BlockTest> block = std::make_shared<BlockTest>();
        block->iVersion = i;
        block->strName = "Block";

        printf("TH_Send Name: %s, Version: %d\n", block->strName.c_str(), block->iVersion);
        queue->put(block);
    }

    return NULL;
}

void *TH_Recv(void *pContext)
{
    CBlockQueue<std::shared_ptr<BlockTest>> *queue = (CBlockQueue<std::shared_ptr<BlockTest>> *)pContext;

    for(int i=0; i<LOOP_NUMS; ++i) {
        std::shared_ptr<BlockTest> block(queue->take());
        printf("TH_Recv Name: %s, Version: %d\n", block->strName.c_str(), block->iVersion);
    }

    return NULL;
}

