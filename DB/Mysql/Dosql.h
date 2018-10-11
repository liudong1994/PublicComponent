#ifndef DO_SQL_H_
#define DO_SQL_H_

#include <mysql/mysql.h>
#include <string>
#include <vector>

using std::string;
using std::vector;

class CDoSql {
public:
    CDoSql() {}
    ~CDoSql() {}
    int Init(const std::string &strHost, int iPort, const std::string &strUser, const std::string &strPasswd, const std::string &strDb, int iRetry);
    int Connect();
    void Close();


public:
    int DoQuery(const string& strSql, vector<vector<string>> &result);
    int DoBinQuery(const string& strSql, string& result);
    int DoUpdate(const string& strSql);
    int DoBinUpdate(const string& strSql, string& strParam);


private:
    bool ReConnect();
    bool ExecuteSQL(const string &strSql);
    bool ExecuteBinSQL(const string &strSql, const string &strParam);

    void AutoCommit(bool bFlag);
    int Commit();
    int RollBack();


private:
    CDoSql(const CDoSql &);
    void operator=(const CDoSql &);

    std::string m_strHost;
    std::string m_strUser;
    std::string m_strPasswd;
    std::string m_strDb;
    int m_iPort;
    int m_iRetry;

    MYSQL m_objMySql;
};

#endif // DO_SQL_H_

