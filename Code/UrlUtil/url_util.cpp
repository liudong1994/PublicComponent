#include <cstring>
#include "url_util.h"

using std::string;
const char CUrlUtil::hex2dec[256] = {
    /*       0  1  2  3   4  5  6  7   8  9  A  B   C  D  E  F */
    /* 0 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 1 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 2 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 3 */  0, 1, 2, 3,  4, 5, 6, 7,  8, 9,-1,-1, -1,-1,-1,-1,
    
    /* 4 */ -1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 5 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 6 */ -1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 8 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    
    /* 8 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 9 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* A */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* B */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    
    /* C */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* D */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* E */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* F */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1
};

const char CUrlUtil::dec2hex[17] = "0123456789ABCDEF";

const char CUrlUtil::safe[256] = {
    /*      0 1 2 3  4 5 6 7  8 9 A B  C D E F */
    /* 0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 1 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 2 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,1,1,0,
    /* 3 */ 1,1,1,1, 1,1,1,1, 1,1,0,0, 0,0,0,0,
    
    /* 4 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
    /* 5 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,1,
    /* 6 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
    /* 7 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,1,0,
    
    /* 8 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 9 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* A */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* B */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    
    /* C */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* D */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* E */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* F */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};

bool CUrlUtil::add_arg(string &url, const char *key, const size_t key_len, const char *value, const size_t val_len, const bool domain) {
    if (!key || !value || key_len == 0) { return false; }

    if (domain) {
        size_t pos = string::npos;
        pos = url.rfind('?');
        if (pos == string::npos) {
            url.append(1, '?');
        }
    }

    if (!url.empty() && url.back() != '&' && url.back() != '?') {
        url.append(1, '&');
    }

    url.append(key, key_len);
    url.append(1, '=');
    url.append(value, val_len);

    return true;
}

bool CUrlUtil::add_arg(string &url, const string &key, const string &value, const bool domain) {
    return add_arg(url, key.c_str(), key.length(), value.c_str(), value.length(), domain);
}

bool CUrlUtil::encode(const string &in, string *out) {
    if (!out) { return false; }

    const unsigned char *p = (const unsigned char *)in.c_str();
    const unsigned char * const e = p + in.length();

    out->clear();
    out->reserve(in.length() * 2);
    for (; p < e; ++ p) {
        if (safe[*p]) {
            out->append(1, *p);
        } else {
            out->append(1, '%');
            out->append(1, dec2hex[*p >> 4 & 0x0f]);
            out->append(1, dec2hex[*p & 0x0f]);
        }
    }

    return true;
}

bool CUrlUtil::decode(const string &in, string *out) {
    if (!out) { return false; }
    const unsigned char *p = (const unsigned char *)in.c_str();
    const unsigned char *const e = p + in.length();
    const unsigned char *const last = e - 2;

    out->clear();
    out->reserve(in.length());

    while (p < last) {
        if (*p == '%') {
            char dec1, dec2;

            dec1 = hex2dec[*(p + 1)];
            dec2 = hex2dec[*(p + 2)];

            if (dec1 != -1 && dec2 != -1) {
                out->append(1, dec1 << 4 | dec2);
                p += 3;
                continue;
            }
        } else if (*p == '+') {
            out->append(1, ' ');
            p++;
            continue;
        }
        
        out->append(1, *p++);
    }

    while (p < e) {
        out ->append(1, *p++);
    }

    return true;
}

