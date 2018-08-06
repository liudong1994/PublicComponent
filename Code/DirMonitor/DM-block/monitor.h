#ifndef _MONITOR_H__
#define _MONITOR_H__

#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <sys/stat.h>

#include <vector>
#include <string>
#include <map>
#include <mutex>
#include <thread>

using namespace std;


class CMonitor {
public:
	CMonitor(void (*func)(inotify_event *));
	~CMonitor();

public:
    // 监控目录操作 dirs传入绝对路径
	int Monitor(const vector<string> &dirs);
    void HandleEvent(inotify_event *event);
	
private:
	int Watch(const string&dir);
	void RmWatch(const string &path);

private:
    const int   m_iNotifyFd;
    std::map<int, string>   monitor_map;         // map<fd, fullpath>

    void        (*m_func)(inotify_event *);
};

#endif

