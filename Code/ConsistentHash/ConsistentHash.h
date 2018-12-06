#ifndef CONSISTENT_HASH_H_
#define CONSISTENT_HASH_H_

#include <string>
#include <map>
#include <memory>

using std::string;
using std::map;
using std::pair;
using std::shared_ptr;


// ��̨������
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
    int     m_iServerNum;           // ��̨����������
    int     m_iVirtualServerNum;    // ÿ̨��������Ҫ �����������

    map<uint64_t, shared_ptr<BackendServer>>  m_mapBackServers;     // ���к�̨������

};

#endif

