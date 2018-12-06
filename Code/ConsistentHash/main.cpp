#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <iostream>

#include "ConsistentHash.h"

using std::vector;
using std::make_shared;


const int USER_COUNT = 100000;
void GenUserId(char *buff, int name_len);


int main()
{
    srand(time(NULL));

    shared_ptr<BackendServer> pBackendServer1 = make_shared<BackendServer> (1);
    shared_ptr<BackendServer> pBackendServer2 = make_shared<BackendServer> (2);
    shared_ptr<BackendServer> pBackendServer3 = make_shared<BackendServer> (3);

    vector<string> vecUserIds;
    for (int i=0; i<USER_COUNT; ++i) {
        char userid[64];
        GenUserId(userid, 64);

        vecUserIds.push_back(string(userid));
    }


    // 1.normal
    shared_ptr<CConsistentHash> pConsistentHash = make_shared<CConsistentHash> (3);
    pConsistentHash->AddBackendServer(pBackendServer1);
    pConsistentHash->AddBackendServer(pBackendServer2);
    pConsistentHash->AddBackendServer(pBackendServer3);
    fprintf(stderr, "------------------ BackendServer number 3 ------------------\n");

    for (int i=0; i<USER_COUNT; ++i) {
        shared_ptr<BackendServer> pBackendServer = pConsistentHash->GetBackendServer(vecUserIds[i]);
        //fprintf(stderr, "Get BackendServer ino:%d\n", pBackendServer->iNo);
    }
    pBackendServer1->PrintAndClearCount();
    pBackendServer2->PrintAndClearCount();
    pBackendServer3->PrintAndClearCount();


    // 2.add
    shared_ptr<BackendServer> pBackendServer4 = make_shared<BackendServer> (4);
    pConsistentHash->AddBackendServer(pBackendServer4);
    fprintf(stderr, "------------------ BackendServer number 4 ------------------\n");

    for (int i=0; i<USER_COUNT; ++i) {
        shared_ptr<BackendServer> pBackendServer = pConsistentHash->GetBackendServer(vecUserIds[i]);
        //fprintf(stderr, "Get BackendServer ino:%d\n", pBackendServer->iNo);
    }
    pBackendServer1->PrintAndClearCount();
    pBackendServer2->PrintAndClearCount();
    pBackendServer3->PrintAndClearCount();
    pBackendServer4->PrintAndClearCount();


    // 3.del
    pConsistentHash->DelBackendServer(pBackendServer2);
    fprintf(stderr, "------------------ BackendServer number 3 ------------------\n");

    for (int i=0; i<USER_COUNT; ++i) {
        shared_ptr<BackendServer> pBackendServer = pConsistentHash->GetBackendServer(vecUserIds[i]);
        //fprintf(stderr, "Get BackendServer ino:%d\n", pBackendServer->iNo);
    }
    pBackendServer1->PrintAndClearCount();
    pBackendServer3->PrintAndClearCount();
    pBackendServer4->PrintAndClearCount();

    return 0;
}

void GenUserId(char *buff, int name_len) {
    static char allChar[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    for (int i = 0; i < name_len - 1; i++) {
        int iRand = rand();
        buff[i] = allChar[iRand % 62];

    }
    buff[name_len - 1] = '\0';
}

