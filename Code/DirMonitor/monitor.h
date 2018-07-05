#ifndef MONITOR_H
#define MONITOR_H

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <sys/stat.h>

#include <cstdio>
#include <vector>
#include <iostream>
#include <string>
#include <set>
#include <map>
#include <mutex>

using namespace std;


struct EVENT{
	int len;
	int wd;
	int mask;
	std::string name;
};


class CMonitor {
public:
	CMonitor();
	~CMonitor();

public:
    // ���Ŀ¼���� dirs�������·��
	int Monitor(const vector<string> &dirs);
	void HandleEvent(struct EVENT *event);

    void Enqueue(EVENT evt);
	
public:
    const int fd;
   	std::map<std::string, EVENT> *event_map;    // map<fullpath, EVENT>
	std::mutex m_mtx;
	

private:
	int Watch(const string&dir);
	void RmWatch(const string &path);

private:
    std::map<int, string> *monitor_map;         // map<fd, fullpath>
	
};

#endif

