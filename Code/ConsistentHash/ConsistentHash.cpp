#include "ConsistentHash.h"
#include <openssl/md5.h>


const int MD5_BYTES = 16;


uint64_t GetMD5(const string &str) {
    char buf[MD5_BYTES] = {0};
    MD5((const unsigned char *)str.c_str(), str.size(), (unsigned char *)buf);
    return *(uint64_t*)buf;
}

CConsistentHash::CConsistentHash(int iVirtualServerNum)
: m_iServerNum(0)
, m_iVirtualServerNum(iVirtualServerNum) {

}

CConsistentHash::~CConsistentHash() {

}

int CConsistentHash::AddBackendServer(shared_ptr<BackendServer> &pBackendServer) {
    m_iServerNum++;

    for (int i=0; i<m_iVirtualServerNum; ++i) {
        const string strServerName = "server_" + std::to_string(pBackendServer->iNo) + "_" + std::to_string(i);
        uint64_t ullMd5Key = GetMD5(strServerName);
        fprintf(stderr, "Add BackendServer ino:%d name:%s md5:%llu success\n", pBackendServer->iNo, strServerName.c_str(), ullMd5Key);
        
        m_mapBackServers.insert(make_pair(ullMd5Key, pBackendServer));
    }
}

int CConsistentHash::DelBackendServer(shared_ptr<BackendServer> &pBackendServer) {
    m_iServerNum--;

    for (int i=0; i<m_iVirtualServerNum; ++i) {
        const string strServerName = "server_" + std::to_string(pBackendServer->iNo) + "_" + std::to_string(i);
        uint64_t ullMd5Key = GetMD5(strServerName);

        auto itr = m_mapBackServers.find(ullMd5Key);
        if (itr != m_mapBackServers.end()) {
            fprintf(stderr, "Del BackendServer ino:%d name:%s md5:%llu success\n", pBackendServer->iNo, strServerName.c_str(), ullMd5Key);
            m_mapBackServers.erase(itr);

        } else {
            fprintf(stderr, "Del BackendServer ino:%d name:%s md5:%llu failed, not exist\n", pBackendServer->iNo, strServerName.c_str(), ullMd5Key);
        }
    }
}

shared_ptr<BackendServer> CConsistentHash::GetBackendServer(const string &strUserId) {
    uint64_t ullMd5Key = GetMD5(strUserId);
    auto itr = m_mapBackServers.lower_bound(ullMd5Key);     // 沿环的顺时针找到一个大于等于key的虚拟节点
    
    if (itr == m_mapBackServers.end()) {
        itr = m_mapBackServers.begin();
    }
    shared_ptr<BackendServer> &pBackendServer = itr->second;
    pBackendServer->iCount++;

    //fprintf(stderr, "UserId:%s backendServer no:%d\n", strUserId.c_str(), pBackendServer->iNo);
    return pBackendServer;
}

