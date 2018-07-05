#include "monitor.h"

static const int EVENTS_TO_WATCH = IN_CREATE | IN_MOVED_TO | IN_DELETE | IN_MODIFY | IN_MOVED_FROM ;
static const int EVENT_SIZE = sizeof(struct inotify_event);
static const uint32_t BUFFER_LEN = 1024 * (EVENT_SIZE * 16);


CMonitor::CMonitor()
: fd(inotify_init())
{
    monitor_map = new map<int,string>();
    event_map = new map<string,EVENT>();
}

CMonitor::~CMonitor() {
    if (monitor_map != nullptr) {
        if (fd > 0) {
            for (map<int,string>::iterator it=monitor_map->begin();it!=monitor_map->end();it++) {
                inotify_rm_watch( fd, it->first );
            }
            close( fd );
        }
        
        delete monitor_map;
    }

    if (event_map != nullptr) {
        event_map->clear();
        delete event_map;
    }
}

void CMonitor::Enqueue(EVENT event) {
    auto it = monitor_map->find(event.wd);
    string path = it->second;
    string name = event.name;
    string full_path = path + "/" + name;

    if (name[0] != '.') {
        m_mtx.lock();

        if (event_map->find(full_path) != event_map->end()) {
            event_map->erase(full_path);
        }
        event_map->insert(make_pair(full_path , event));
                
        m_mtx.unlock();
    }
}

void * TH_Sentinel(void * args){
    if (args == nullptr) {
        printf("TH_Sentinel args is null\n");
        return NULL;
    }

    CMonitor *monitor = (CMonitor *)args;
    
    try{
        printf("[Monitor] MONITOR SENTINEL THREAD START\n");

        while (1) {
            if (monitor->event_map->size() > 0) {

                monitor->m_mtx.lock();
                std::map<std::string, EVENT> e_map(monitor->event_map->begin(), monitor->event_map->end());
                monitor->event_map->clear();
                monitor->m_mtx.unlock();

                for (auto it=e_map.begin(); it!=e_map.end(); it++) {
                    printf("HandleEvent Path: %s\n", it->first.c_str());
                    monitor->HandleEvent(&(it->second));
                }
            }

            sleep(1);
        }
    } catch(...) {
        printf("[Initiator] monitor error\n");
    }

    return NULL;
}

int CMonitor::Monitor(const vector<string> &dirs) {
    if (fd <= 0 || dirs.size() <= 0) {
        printf("Monitor Start failed, param ERROR\n");
        return -1;
    }

    for(auto it=dirs.begin(); it!=dirs.end(); it++) {
        Watch(*it);
    }

#ifdef __DEBUG__
    for (auto idx = monitor_map->begin();idx!=monitor_map->end();idx++) {
        printf("[Monitor] <monitor-pair> %d: %s\n", idx->first,idx->second.c_str());
    }
#endif

    pthread_t t;
    if (pthread_create(&t, NULL, TH_Sentinel, this) != 0) {
        printf("[Monitor] start monitor sentinel thread error\n");
    }

    int start = 0;
    int end = 0;
    char buffer[BUFFER_LEN];

    while(1) {
        start = 0;
        end = read(fd, buffer, BUFFER_LEN);

        if (end <= 0) {
            printf("[Monitor] read event error.ERRNO : %d\n", errno);
        }

        while (start < end) {
            struct inotify_event *event = (struct inotify_event *)&buffer[start];
            std::string name = event->name;

            if (name.size() > 2) {
                EVENT e;
                e.len = event->len;
                e.wd = event->wd;
                e.mask = event->mask;
                e.name = event->name;
                Enqueue(e);
            }
            start += EVENT_SIZE + event->len;
        }
    }

    return 0;
}

int CMonitor::Watch(const string &dir) {
    if (fd < 0 || dir.empty()) {
        printf("[Monitor] inotify init error. failed to start monitor\n");
        return -1;
    }

    int wd = inotify_add_watch(fd, dir.c_str(), EVENTS_TO_WATCH);
    monitor_map->insert(make_pair(wd, dir));

    DIR* dir_p = nullptr;
    if (!(dir_p=opendir(dir.c_str()))) {
        printf("[Monitor] open dir : %s failed\n", dir.c_str());
        return -1;
    }

    chdir(dir.c_str());

    struct dirent* dirent_p = nullptr;
    struct stat buf;
    while ((dirent_p=readdir(dir_p)) != NULL) {
        lstat(dirent_p->d_name, &buf);

        string name(dirent_p->d_name);
        string f_path = dir + "/" + name;

        // 对子目录继续处理
        if (S_ISDIR(buf.st_mode)) {
            if (name!="." && name!="..") {
                Watch(f_path.c_str());
            }
        }
    }

    chdir("..");
    closedir(dir_p);
    printf("[Monitor] watched dir: %s on wd : %d\n", dir.c_str(), wd);

    return wd;
}

void CMonitor::RmWatch(const string & path) {
    int cwd = -1;
    for (map<int,string>::iterator it=monitor_map->begin(); it!=monitor_map->end(); it++) {
        if (it->second == path) {
            cwd = it->first;
            break;
        }
    }

    if (cwd == -1) {
        printf("[Monitor] can not find watcher description for path : %s\n",path.c_str());
    }
    else {
        monitor_map->erase(cwd);
        inotify_rm_watch(fd, cwd);
        printf("[Monitor] --- remove watcher at path : %s\n", path.c_str());
    }
}

void CMonitor::HandleEvent(struct EVENT * event)
{
    if (!event || event->len <= 0) {
        printf("Handle Event Failed\n");
        return ;
    }

    auto it = monitor_map->find(event->wd);
    if (it == monitor_map->end()) {
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
    } else {
        printf("Handle Event name: %s error\n", name.c_str());
    }
}

