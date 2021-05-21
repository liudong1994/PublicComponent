#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <thread>
#include <sys/time.h>
#include <ctime>

#include "async_timer.h"

using std::string;


struct Context 
{
    int value = 0;
};

void async_handler(Context *ctx)
{
    printf("async handler ctx value:%d\n", ctx->value);

    delete ctx;
    ctx = nullptr;

    return;
}

int main()
{
	CAsyncTimerTask<Context *>* _async = new CAsyncTimerTask<Context *>(async_handler, 1024, 4, 1000);


    Context* ctx = new Context;
    ctx->value = 1;
    _async->add_task(ctx);

    sleep(10);

    return 0;
}

