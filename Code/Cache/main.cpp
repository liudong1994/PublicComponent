#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#include "cache.h"


typedef CACHE::Cache<unsigned long long> CacheUINT64;
typedef std::shared_ptr<CacheUINT64> CacheSptr;


int main()
{
    std::string strKeyId = "key_123456";
    unsigned long long ullPrice = 50000;
    unsigned long long ullCachedPrice = 0;


    CacheSptr pCache = std::make_shared<CacheUINT64>();
    pCache->set(strKeyId, 666, 1 * 3600);
    std::cout << pCache->print();
    
    pCache->incrby(strKeyId, ullPrice, ullCachedPrice, 24 * 3600);
    pCache->set(strKeyId + "7", 34567, 24 * 3600);
    std::cout << pCache->print();

    sleep(1);
    pCache->del(strKeyId);
    std::cout << pCache->print();

    return 0;
}

