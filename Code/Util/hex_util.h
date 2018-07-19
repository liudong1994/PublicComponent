#ifndef _HEX_UTIL_H_
#define _HEX_UTIL_H_

#include <string>
#include <strings.h>
using std::string;


class CHexUtil {
public:
    CHexUtil() = delete;

public:
    static string Hex2Byte(const string & hexmsg);

};

#endif /* _HEX_UTL_H_ */

