#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timer_task.h"


class ClassTemp {
public:
    ClassTemp() {}
    ~ClassTemp() {}

    void init()
    {
        // 2s定时任务
        m_timer_task.start(std::bind(&ClassTemp::offline_job, this), 2000, false);
    }

private:
    void offline_job() 
    {
        printf("ClassTemp offline job\n");
    }

private:
    TimerTask m_timer_task;
};

int main()
{
    ClassTemp temp;
    temp.init();
    
    while (1) {
        sleep(1);
    }
    return 0;
}

