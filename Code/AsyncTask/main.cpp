#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <thread>     // C++11

#include "AsyncTask.h"

using std::string;

#define LOOP_NUMS 100

struct BlockTest {
    int iVersion;
    string strName;

    BlockTest(): iVersion(0) {

    }
};

void Async_handler(std::shared_ptr<BlockTest> pBlock);


int main()
{
    std::shared_ptr<CAsyncTask<std::shared_ptr<BlockTest>>> pAsyncTask = std::make_shared<CAsyncTask<std::shared_ptr<BlockTest>>>(Async_handler, 2048, 1);

    for(int i=0; i<LOOP_NUMS; ++i) {
        std::shared_ptr<BlockTest> block = std::make_shared<BlockTest>();
        block->iVersion = i;
        block->strName = "Block";

        printf("TH_Send Name: %s, Version: %d\n", block->strName.c_str(), block->iVersion);
        pAsyncTask->add_task(block);
    }

    printf("wait process block start\n");
    sleep(1);
    printf("wait process block end\n");

    return 0;
}

void Async_handler(std::shared_ptr<BlockTest> pBlock)
{
    printf("Async_handler Name: %s, Version: %d\n", pBlock->strName.c_str(), pBlock->iVersion);
}

