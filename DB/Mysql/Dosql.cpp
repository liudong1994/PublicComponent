#include "Dosql.h"
#include <stdlib.h>
#include <string.h>

const int MYSQL_OK = 0;
const int MYSQL_ERROR = -1;


const string MYSQL_SET_NAMES = "set names \'utf8\'";

int CDoSql::Init(const std::string &strHost, int iPort, const std::string &strUser, const std::string &strPasswd, const std::string &strDb, int iRetry) {
    m_strHost = strHost;
    m_strUser = strUser;
    m_strPasswd = strPasswd;
    m_strDb = strDb;
    m_iPort = iPort;
    m_iRetry = iRetry;

    return MYSQL_OK;
}

int CDoSql::Connect() {
    if (m_strHost.empty() || m_strUser.empty() || m_strPasswd.empty() || m_strDb.empty() || m_iPort == 0) {
        fprintf(stderr, "init mysql args error.");
        return MYSQL_ERROR;
    }

    MYSQL *pConnptr = mysql_init(&m_objMySql);
    if (!pConnptr) {
        fprintf(stderr, "mysql_init failed.");
        return MYSQL_ERROR;
    }

    // 设置utf8字符集
    if (0 != mysql_options(&m_objMySql, MYSQL_SET_CHARSET_NAME, "utf8")) {
        fprintf(stderr, "mysql_options MYSQL_SET_CHARSET_NAME failed.");
        return MYSQL_ERROR;
    }

    pConnptr = mysql_real_connect(&m_objMySql, m_strHost.c_str(), m_strUser.c_str(), m_strPasswd.c_str(), m_strDb.c_str(), m_iPort, NULL, 0);
    if (!pConnptr) {
        fprintf(stderr, "connection error %d: %s!", mysql_errno(&m_objMySql), mysql_error(&m_objMySql));
        return MYSQL_ERROR;
    }

    return MYSQL_OK;
}

void CDoSql::Close() {
    mysql_close(&m_objMySql);
}

bool CDoSql::ReConnect() {
    Close();
    return Connect();
}

bool CDoSql::ExecuteSQL(const string &strSql) {
    mysql_real_query(&m_objMySql, MYSQL_SET_NAMES.c_str(), MYSQL_SET_NAMES.length());

    if (mysql_real_query(&m_objMySql, strSql.c_str(), strSql.length()) != 0) {
        fprintf(stderr, "mysql_real_query error %d: %s!", mysql_errno(&m_objMySql), mysql_error(&m_objMySql));
        return MYSQL_ERROR;
    }

    return MYSQL_OK;
}

bool CDoSql::ExecuteBinSQL(const string &strSql, const string &strParam) {
    mysql_real_query(&m_objMySql, MYSQL_SET_NAMES.c_str(), MYSQL_SET_NAMES.length());

    int length = strParam.length();
    char chunk[2 * length + 1];
    mysql_real_escape_string(&m_objMySql, chunk, strParam.c_str(), length);

    char* pSql = const_cast<char*>(strSql.c_str());
    size_t uSqlLen = strlen(pSql);

    char querystr[uSqlLen + 2 * length + 1];
    int len = snprintf(querystr, uSqlLen + 2 * length + 1, pSql, chunk);

    if (mysql_real_query(&m_objMySql, querystr, len) != 0) {
        fprintf(stderr, "mysql_real_query error %d: %s!", mysql_errno(&m_objMySql), mysql_error(&m_objMySql));
        return MYSQL_ERROR;
    }

    return MYSQL_OK;
}

void CDoSql::AutoCommit(bool bFlag) {
    mysql_autocommit(&m_objMySql, bFlag);
}

int CDoSql::Commit() {
    return mysql_commit(&m_objMySql);
}

int CDoSql::RollBack() {
    return mysql_rollback(&m_objMySql);
}

// ---------------------------------------------
int CDoSql::DoQuery(const string& strSql, vector<vector<string>> &vecResult) {
    int iQueryResult = -1;
    for (int i=0; i<m_iRetry; ++i) {
        iQueryResult = ExecuteSQL(strSql.c_str());
        if (MYSQL_OK == iQueryResult) {
            break;
        }

        fprintf(stderr, "reconnect, query error %d: %s: sql: %s", mysql_errno(&m_objMySql), mysql_error(&m_objMySql), strSql.c_str());
        ReConnect();
    }

    if (0 != iQueryResult) {
        fprintf(stderr, "query error %d: %s: sql: %s", mysql_errno(&m_objMySql), mysql_error(&m_objMySql), strSql.c_str());
        return MYSQL_ERROR;
    }

    int iRet = MYSQL_OK;
    MYSQL_RES * pResptr = mysql_store_result(&m_objMySql);

    if (pResptr != NULL) {
        int row = mysql_num_rows(pResptr);     //行
        int col = mysql_num_fields(pResptr);   //列

        if (row && col) {
            for (int i = 0; i < row; i++) {
                MYSQL_ROW sqlrow = mysql_fetch_row(pResptr);
                if (NULL == sqlrow) {
                    fprintf(stderr, "mysql_fetch_row error row: %d  col: %d sql: %s", row, col, strSql.c_str());
                    iRet = MYSQL_ERROR;
                    break;
                }

                vector<string> vecValues;
                for (int j = 0; j < col; j++) {
                    vecValues.push_back(sqlrow[j]);
                }
                vecResult.push_back(vecValues);
            }
        } else {
            fprintf(stderr, "error row: %d  col: %d sql: %s", row, col, strSql.c_str());
            iRet = MYSQL_ERROR;
        }

        mysql_free_result(pResptr);   // 完成对数据的所有操作后,调用此函数来让MySQL库清理它分>配的对象
    } else {
        fprintf(stderr, "mysql_store_result error %d: %s!", mysql_errno(&m_objMySql), mysql_error(&m_objMySql));
        iRet = MYSQL_ERROR;
    }

    return iRet;
}

int CDoSql::DoBinQuery(const string& strSql, string& strResult) {
    int iQueryResult = -1;
    for (int i=0; i<m_iRetry; ++i) {
        iQueryResult = ExecuteSQL(strSql.c_str());
        if (0 == iQueryResult) {
            break;
        }

        fprintf(stderr, "reconnect, query error %d: %s: sql: %s", mysql_errno(&m_objMySql), mysql_error(&m_objMySql), strSql.c_str());
        ReConnect();
    }

    if (0 != iQueryResult) {
        fprintf(stderr, "query error %d: %s: sql: %s", mysql_errno(&m_objMySql), mysql_error(&m_objMySql), strSql.c_str());
        return MYSQL_ERROR;
    }


    int iRet = MYSQL_OK;
    MYSQL_RES * pResptr = mysql_store_result(&m_objMySql);

    if (pResptr != NULL) {
        MYSQL_ROW sqlrow = mysql_fetch_row(pResptr);
        if(sqlrow != NULL) {
            unsigned long *len = mysql_fetch_lengths(pResptr);
            if (len != NULL) {
                strResult.assign(sqlrow[0], *len);
            } else {
                fprintf(stderr, "mysql_fetch_lengths error sql: %s", strSql.c_str());
                iRet = MYSQL_ERROR;
            }
        } else {
            fprintf(stderr, "mysql_fetch_row error sql: %s", strSql.c_str());
            iRet = MYSQL_ERROR;
        }

        mysql_free_result(pResptr);      // 完成对数据的所有操作后,调用此函数来让MySQL库清理它分>配的对象
    } else {
        fprintf(stderr, "mysql_store_result error %d: %s!", mysql_errno(&m_objMySql), mysql_error(&m_objMySql));
        iRet = MYSQL_ERROR;
    }

    return iRet;
}

int CDoSql::DoUpdate(const string& strSql) {
    int iQueryResult = -1;
    for (int i=0; i<m_iRetry; ++i) {
        iQueryResult = ExecuteSQL(strSql.c_str());
        if (0 == iQueryResult) {
            break;
        }

        fprintf(stderr, "reconnect, query error %d: %s: sql: %s", mysql_errno(&m_objMySql), mysql_error(&m_objMySql), strSql.c_str());
        ReConnect();
    }

    if (0 != iQueryResult) {
        fprintf(stderr, "query error %d: %s: sql: %s", mysql_errno(&m_objMySql), mysql_error(&m_objMySql), strSql.c_str());
        return MYSQL_ERROR;
    }

    return MYSQL_OK;
}

int CDoSql::DoBinUpdate(const string& strSql, string& strParam) {
    int iQueryResult = -1;
    for (int i=0; i<m_iRetry; ++i) {
        iQueryResult = ExecuteBinSQL(strSql.c_str(), strParam);
        if (0 == iQueryResult) {
            break;
        }

        fprintf(stderr, "reconnect, query error %d: %s: sql: %s", mysql_errno(&m_objMySql), mysql_error(&m_objMySql), strSql.c_str());
        ReConnect();
    }

    if (0 != iQueryResult) {
        fprintf(stderr, "query error %d: %s: sql: %s", mysql_errno(&m_objMySql), mysql_error(&m_objMySql), strSql.c_str());
        return MYSQL_ERROR;
    }

    return MYSQL_OK;
}

