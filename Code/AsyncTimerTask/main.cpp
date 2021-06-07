#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <thread>
#include <chrono>
#include "async_timer_task.h"

using std::string;


struct Context 
{
    uint64_t value = 0;
};

void async_handler(Context *ctx)
{
    uint64_t time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - ctx->value;

    if (time_diff > 2002) {
        printf("async handler ctx value:%lu  diff:%lu\n", ctx->value, time_diff);
    }

    delete ctx;
    ctx = nullptr;

    return;
}

int main()
{
	CAsyncTimerTask<Context *>* _async = new CAsyncTimerTask<Context *>(async_handler, 1024000, 4);

    for (int i=0; i<100000; ++i) {
        Context* ctx = new Context;
        ctx->value = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        _async->add_task(ctx, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() + 2000);
        usleep(1);
    }

    sleep(10);

    delete _async;

    return 0;
}

