#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tcp.h"


int main()
{
    CTcp *pTcp = new CTcp(5011, "192.168.144.118", 500);
    pTcp->set_nodelay();
    pTcp->client_connect();

    // send string
    std::string strRequest = "POST /imp/adapter/zyb HTTP/1.1\r\nContent-Type: application/json\r\nContent-Length: 1383\r\nHost: up_imp_adapter\r\n\r\n{\"version\":\"1.0\",\"is_test\":true,\"id\":\"6d66e384-638e-4778-a217-a9a540405f15\",\"user\":{\"user_agent\":\"Mozilla\/5.0 (Linux; Android 9; GLK-AL00 Build\/HUAWEIGLK-AL00; wv) AppleWebKit\/537.36 (KHTML, like Gecko) Version\/4.0 Chrome\/70.0.3538.64 Mobile Safari\/537.36\",\"ip\":\"117.173.98.74\",\"id\":\"3097E009EE263BAE1AD660991A003D1E|O\",\"province_code\":\"102107510000\",\"city_code\":\"102107513400\"},\"adSlot\":[{\"deal_type\":\"RTB\",\"id\":\"c74ae4e4-f917-4f7e-a34e-4bbb2bc5cfb6\",\"slot_visibility\":1,\"banner\":{\"templateId\":[10041,10042,10048,10049,10064,10065,10066,10067,10070,10071,10072,10073,10074,10094,10091,10092,10093,10120,10121],\"width\":700,\"height\":200,\"view_type\":[304]},\"search\":{\"ori_wd\":\"\",\"st_wd\":\"\"},\"excluded_adx_advertiserid\":[1453,1452],\"slotid\":\"207\",\"min_cpm_price\":0}],\"loadid\":\"\",\"is_debug\":false,\"site\":{\"pageid\":\"82266\"},\"uniqid\":\"\",\"mobile\":{\"device\":{\"screen_orientation\":0,\"lat\":27897586,\"conn\":1,\"screen_density\":3,\"screen_hight\":2208,\"screen_width\":1080,\"lng\":102238197,\"deviceid\":\"IB3X\/Pj8YEaMLFD72XkMtMOwpw==\",\"devicemodel\":\"GLK-AL00\",\"mac\":\"02:00:00:00:00:00\",\"devicebrand\":\"HUAWEI\",\"pm\":2,\"networkid\":7012,\"os_version\":\"9\"},\"pkgname\":\"com.baidu.homework\",\"v\":\"11.14.4\",\"avc\":\"593\",\"is_app\":true},\"ps_resp\":{\"tsubjectid\":\"2\",\"tkpotids\":\"f8d4f670972c3cde1ba2fb7076a2169e,f8d4f670972c3cde1ba2fb7076a2169e\",\"tgradeid\":\"1\",\"tid\":\"f66826d3be6aa117f37e9df567d832d9\"},\"is_test\":false}";
    printf("send string:\n%s\n", strRequest.c_str());

	int i=0;
	while(1) {
		pTcp->tcp_writeall(strRequest.c_str(), strRequest.size());

        i++;
        if (i >= 10) {
             printf("send %d times, close tcp scoket\n", i);
             pTcp->tcp_close();
             break;
        }

        char buf[8096] = { 0 };
		int ret = pTcp->tcp_read(buf, 8096);
		if (ret == -2) {
            printf("send error, close tcp scoket\n");
		    pTcp->tcp_close();
		    break;
        }
	}

    while (1) {
        sleep(1);
    }
    return 0;
}

