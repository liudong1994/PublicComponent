#include "monitor.h"

static const int EVENTS_TO_WATCH = IN_CREATE | IN_MOVED_TO | IN_DELETE | IN_MODIFY | IN_MOVED_FROM ;
static const int EVENT_SIZE = sizeof(struct inotify_event);
static const uint32_t BUFFER_LEN = 1024 * (EVENT_SIZE * 16);


CMonitor::CMonitor(void (*func)(inotify_event *))
: m_iNotifyFd(inotify_init())
, m_func(func) {

}

CMonitor::~CMonitor() {
    if (m_iNotifyFd > 0) {
        for (auto it=monitor_map.begin(); it!=monitor_map.end(); it++) {
            inotify_rm_watch( m_iNotifyFd, it->first );
        }
        close( m_iNotifyFd );
    }
}

int CMonitor::Monitor(const vector<string> &dirs) {
    if (m_iNotifyFd <= 0 || dirs.size() <= 0) {
        printf("Monitor Start failed, param ERROR\n");
        return -1;
    }

    for(auto it=dirs.begin(); it!=dirs.end(); it++) {
        Watch(*it);
    }

#ifdef __DEBUG__
    for (auto idx = monitor_map.begin();idx!=monitor_map.end();idx++) {
        printf("[Monitor] <monitor-pair> %d: %s\n", idx->first,idx->second.c_str());
    }
#endif

    int start = 0;
    int end = 0;
    char buffer[BUFFER_LEN];

    while(1) {
        start = 0;
        end = read(m_iNotifyFd, buffer, BUFFER_LEN);
        if (end <= 0) {
            printf("[Monitor] read event error.ERRNO : %d\n", errno);
        }

        while (start < end) {
            inotify_event *event = (inotify_event *)&buffer[start];
            std::string strName(event->name);

            if (strName.size() > 2 && strName[0] != '.') {
                HandleEvent(event);
            }
            start += EVENT_SIZE + event->len;
        }
    }

    return 0;
}

int CMonitor::Watch(const string &strDir) {
    if (m_iNotifyFd < 0 || strDir.empty()) {
        printf("[Monitor] failed to start Watch\n");
        return -1;
    }

    int iWd = inotify_add_watch(m_iNotifyFd, strDir.c_str(), EVENTS_TO_WATCH);
    monitor_map.insert(make_pair(iWd, strDir));

    DIR* dir_p = nullptr;
    if (!(dir_p=opendir(strDir.c_str()))) {
        printf("[Monitor] open dir: %s failed\n", strDir.c_str());
        return -1;
    }

    chdir(strDir.c_str());

    struct dirent* dirent_p = nullptr;
    struct stat buf;
    while ((dirent_p=readdir(dir_p)) != NULL) {
        lstat(dirent_p->d_name, &buf);

        string name(dirent_p->d_name);
        string f_path = strDir + "/" + name;

        // 对子目录继续处理
        if (S_ISDIR(buf.st_mode)) {
            if (name!="." && name!="..") {
                Watch(f_path.c_str());
            }
        }
    }

    chdir("..");
    closedir(dir_p);
    printf("[Monitor] watched dir: %s on wd : %d\n", strDir.c_str(), iWd);

    return iWd;
}

void CMonitor::RmWatch(const string & path) {
    int cwd = -1;
    for (auto it=monitor_map.begin(); it!=monitor_map.end(); it++) {
        if (it->second == path) {
            cwd = it->first;
            break;
        }
    }

    if (cwd == -1) {
        printf("[Monitor] can not find watcher description for path : %s\n",path.c_str());
    } else {
        monitor_map.erase(cwd);
        inotify_rm_watch(m_iNotifyFd, cwd);
        printf("[Monitor] --- remove watcher at path : %s\n", path.c_str());
    }
}

void CMonitor::HandleEvent(inotify_event *event)
{
    if (!event || event->len <= 0) {
        printf("Handle Event Failed\n");
        return ;
    }

    auto it = monitor_map.find(event->wd);
    if (it == monitor_map.end()) {
        printf("Handle Event event wd failed\n");
        return ;
    }

    string path = it->second;
    string name = event->name;
    string full_path = path + "/" + name;

    if (name[0] != '.') {
        if ( event->mask & IN_CREATE ) {
            if ( event->mask & IN_ISDIR ) {
                Watch(full_path);
                printf("path: %s name: %s CREATE\n", path.c_str(), name.c_str());
            }
            else {
                printf("path: %s name: %s CREATE\n", path.c_str(), name.c_str());
            }
        }
        else if ( event->mask & IN_MOVED_TO ) {
            if ( event->mask & IN_ISDIR ) {
                Watch(full_path);
                printf("path: %s name: %s MOVEIN\n", path.c_str(), name.c_str());
            }
            else {
                printf("path: %s name: %s MOVEIN\n", path.c_str(), name.c_str());
            }
        }
        else if ( event->mask & IN_DELETE ) {
            if ( event->mask & IN_ISDIR ) {
                RmWatch(full_path);
                printf("path: %s name: %s DELETE\n", path.c_str(), name.c_str());
            }
            else {
                printf("path: %s name: %s DELETE\n", path.c_str(), name.c_str());
            }
        }
        else if ( event->mask & IN_MODIFY ) {
            if ( event->mask & IN_ISDIR ) {
                printf("path: %s name: %s MODIFY\n", path.c_str(), name.c_str());
            }
            else {
                printf("path: %s name: %s MODIFY\n", path.c_str(), name.c_str());
            }
        }
        else if ( event->mask & IN_MOVED_FROM ) {
            if ( event->mask & IN_ISDIR ) {
                RmWatch(full_path);
                printf("path: %s name: %s MOVEFROM\n", path.c_str(), name.c_str());
            } 
            else {
                printf("path: %s name: %s MOVEFROM\n", path.c_str(), name.c_str());
            }
        }

        if (m_func) {
            m_func(event);
        }

    } else {
        printf("Handle Event name: %s error\n", name.c_str());
    }
}

