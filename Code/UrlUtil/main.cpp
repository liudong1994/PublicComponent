#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#include "url_util.h"


int main()
{
    std::string strTestUrl = "https://192.168.10.1/urlUtil";
    CUrlUtil::add_arg(strTestUrl, "param", "1");
    CUrlUtil::add_arg(strTestUrl, "key", "2%");
    CUrlUtil::add_arg(strTestUrl, "type", "=3%3");
    printf("Url: %s\n", strTestUrl.c_str());


    std::string strUrlEncode;
    CUrlUtil::encode(strTestUrl, &strUrlEncode);
    printf("EncodeUrl: %s\n", strUrlEncode.c_str());

    std::string strUrlDecode;
    CUrlUtil::decode(strUrlEncode, &strUrlDecode);
    printf("DecodeUrl: %s\n", strUrlDecode.c_str());

    return 0;
}

