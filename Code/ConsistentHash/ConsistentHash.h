#ifndef CONSISTENT_HASH_H_
#define CONSISTENT_HASH_H_

#include <string>
#include <map>
#include <memory>

using std::string;
using std::map;
using std::pair;
using std::shared_ptr;


// 后台服务器
struct BackendServer {
    int iNo;
    int iCount;

    BackendServer(int no): iNo(no), iCount(0) {

    }

    void PrintAndClearCount() {
        fprintf(stderr, "Server no:%d count:%d\n", iNo, iCount);
        iCount = 0;
    }
};


class CConsistentHash {
public:
    explicit CConsistentHash(int iVirtualServerNum);
    ~CConsistentHash();


    int AddBackendServer(shared_ptr<BackendServer> &pBackendServer);
    int DelBackendServer(shared_ptr<BackendServer> &pBackendServer);

    shared_ptr<BackendServer> GetBackendServer(const string &strUserId);


private:
    CConsistentHash(const CConsistentHash & rh);
    void operator=(const CConsistentHash & rh);


private:
    int     m_iServerNum;           // 后台服务器数量
    int     m_iVirtualServerNum;    // 每台服务器需要 虚拟出的数量

    map<uint64_t, shared_ptr<BackendServer>>  m_mapBackServers;     // 所有后台服务器

};

#endif

