#ifndef _URL_UTIL_H_
#define _URL_UTIL_H_

#include <string>


class CUrlUtil {
    public:
        CUrlUtil() = delete;

    public:
        static bool add_arg(std::string &url, const std::string &key, const std::string &value, const bool domain = true);
        static bool add_arg(std::string &url, const char *key, const size_t key_len,
            const char *value, const size_t val_len, const bool doamin = true); 

        static bool encode(const std::string &in, std::string *out);

        static bool decode(const std::string &in, std::string *out);

    private:
        static const char hex2dec[256];
        static const char dec2hex[17];
        static const char safe[256];
};

#endif /* _URL_UTL_H_ */

