#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#include "url_util.h"
#include "hex_util.h"
#include "time_util.h"


int main()
{
    printf("----------------url_util test----------------\n");
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


    printf("----------------hex_util test----------------\n");
    string strHex = "0x0A08323031383037313810011A5808E209122508CE011201321A05313030303222160A06313032313037120C3130323130373131303030301A2808F54E10E94E18FC0720142800320832303138303731383801420D08FD4E10AC0B1A05313030303220012801";
    string strBytes = CHexUtil::Hex2Byte(strHex);
    printf("Hex(%d): %s\nBytes(%d): %s\n", (int)strHex.size(), strHex.c_str(), (int)strBytes.size(), strBytes.c_str());


    printf("----------------time_util test----------------\n");
    printf("CurTimeS:%ld  CurTimeMS:%ld  CurTimeUS:%ld  CurFormatTime:%s\n", GetCurTimeS(), GetCurTimeMS(), GetCurTimeUs(), GetCurFromatTime().c_str());
    printf("FormatTime:%s  TimeS:%ld\n", "2020-01-15 17:26:30", GetTimeSFromFormat("2020-01-15 17:26:30"));

    return 0;
}

