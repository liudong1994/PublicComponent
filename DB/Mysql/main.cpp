#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string>

#include "Dosql.h"

using namespace std;

int main(int argc, char **argv)
{
    if(argc < 6) {
        printf("Usage: ./Test MysqlIP MysqlPort User Passwd Db\n");
        printf("eg: ./Test 192.168.240.138 3306 root root dsp_dev_test1\n");
        return 0;
    }

    CDoSql objDoSql;

    // 连接Redis数据库
    printf("Mysql Info: %s %s %s %s %s\n", argv[1], argv[2], argv[3], argv[4], argv[5]);
    objDoSql.Init(argv[1], atoi(argv[2]), argv[3], argv[4], argv[5], 3);
    if (0 != objDoSql.Connect()) {
        printf("Mysql connect failed\n");
        return -1;
    }
    
    printf("select status, budget from customer_status where customerid = \"1014\"\n");
    vector< vector<string> > vecResult;
    string strSql = "select status, budget from customer_status where customerid = \"1014\"";
    if (0 == objDoSql.DoQuery(strSql, vecResult)) {
        for (size_t i=0; i<vecResult.size(); ++i) {
            for(size_t j=0; j<vecResult[i].size(); ++j) {
                printf("%s\n", vecResult[i][j].c_str());
            }
        }
    } else {
        printf("Mysql DoQuery failed\n");
    }

    // ToDo 增加其他Sql语句测试


    return 0;
}

