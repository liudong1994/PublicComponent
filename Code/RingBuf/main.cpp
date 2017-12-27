#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "RingBuffer.h"
#include "RingBuf.h"


struct TestNode
{
    char    *pData;
    int     nLen;

    TestNode(): pData(NULL), nLen(0)
    {
    }
};

int main()
{
    CRingBuf							ringData;
    CRingBuffer<TestNode, 300>		    nodeRing;

    ringData.Init(102400);
    nodeRing.SetBufferName("Test Node Ring");


    char bufName[1024];
    while(gets(bufName) != NULL && 0 != strcmp(bufName, "quit"))
    {
        printf("input: %s\n", bufName);

        TestNode node;
        node.nLen = strlen(bufName);
        node.pData = ringData.Write(bufName, node.nLen);

        if (NULL != node.pData)
        {
            nodeRing.Push(node);
        }
        else
        {
            printf("node ring add failed\n");
        }
    }

    printf("--------------------------------------\n");

    TestNode node;
    while (nodeRing.Pop(node))
    {
        printf("just input(%d)->%s\n", node.nLen, node.pData);
        ringData.SetRead(node.pData);
    }

    return 0;
}

