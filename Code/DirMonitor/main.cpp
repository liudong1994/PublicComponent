#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#include "monitor.h"



int main()
{
    vector<string> vecDir;
    vecDir.push_back("/home/homework/Monitor");

    CMonitor monitor;
    monitor.Monitor(vecDir);

    return 0;
}

