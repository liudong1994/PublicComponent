#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#include "monitor.h"


static void HandleEvent(inotify_event *event);

int main()
{
    vector<string> vecDir;
    // ��Ҫ����Ŀ¼  �ļ����в���Ч����
    vecDir.push_back("/home/homework/Monitor");

    CMonitor monitor(HandleEvent);
    monitor.Monitor(vecDir);

    return 0;
}

void HandleEvent(inotify_event *event) {
    string name = event->name;
    printf("HandleEvent name: %s\n", name.c_str());
}

