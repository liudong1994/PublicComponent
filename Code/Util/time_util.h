#ifndef _TIME_UTIL_H_
#define _TIME_UTIL_H_

#include <string>


// 获取当前时间 秒
time_t GetCurTimeS();

// 获取当前时间 毫秒
long  GetCurTimeMS();
// 获取当前时间 微妙
long  GetCurTimeUs();

// 将指定格式时间字符串 转换为秒
time_t GetTimeSFromFormat(const std::string &strTimeFormat);

// 获取当前时间的 格式化字符串
std::string GetCurFromatTime();

#endif /* _TIME_UTIL_H_ */