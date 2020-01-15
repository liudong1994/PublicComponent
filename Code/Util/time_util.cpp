#include "time_util.h"
#include <sys/time.h>


time_t GetCurTimeS()
{
    time_t tNow = time(0);
    struct tm *tmNow = localtime(&tNow);
    time_t tTimestamp = mktime(tmNow);

    return tTimestamp;
}

long  GetCurTimeMS()
{
    struct timeval tvNow;
    gettimeofday(&tvNow, NULL);
    return tvNow.tv_sec * 1000 + tvNow.tv_usec / 1000;
}

long  GetCurTimeUs()
{
    struct timeval tvNow;
    gettimeofday(&tvNow, NULL);
    return tvNow.tv_sec * 1000000 + tvNow.tv_usec;
}

time_t GetTimeSFromFormat(const std::string &strTimeFormat)
{
    // 格式化时间
    struct tm tmTime;
    strptime(strTimeFormat.c_str(), "%Y-%m-%d %H:%M:%S", &tmTime);
    time_t tTimestamp = mktime(&tmTime);

    return tTimestamp;
}

std::string GetCurFromatTime()
{
    tm tmTime;
    time_t tNow = time(0); 
    localtime_r(&tNow, &tmTime);
    // tmTime = localtime(&tNow);

    char buff[32] = {0};      
    snprintf(buff, sizeof(buff), "%04d-%02d-%02d %02d:%02d:%02d", tmTime.tm_year + 1900, tmTime.tm_mon + 1, tmTime.tm_mday, tmTime.tm_hour, tmTime.tm_min, tmTime.tm_sec);
    return std::string(buff);
}

