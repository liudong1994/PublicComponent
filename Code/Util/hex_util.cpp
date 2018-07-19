#include "hex_util.h"

string CHexUtil::Hex2Byte(const string & hexmsg) {
    std::string bytes;
    size_t msg_size = hexmsg.size();
    if (msg_size < 2) {
        return bytes;
    }

    size_t i = 0;
    if (!strncasecmp(hexmsg.c_str(), "0x", 2)) {
        i += 2;
    }

    bytes.reserve(msg_size / 2);
    char left = 0, right = 0, bit = 0;
    for (; i + 1 < msg_size; i += 2) {
        bit = toupper(hexmsg[i]);
        left = bit >= 'A' ? bit - 'A' + 10 : bit - '0';

        bit = toupper(hexmsg[i + 1]);
        right = bit >= 'A' ? bit - 'A' + 10 : bit - '0';

        bytes += (left << 4) | right;
    }

    return bytes;
}

