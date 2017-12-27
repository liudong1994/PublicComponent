/*
	Redis数据库接口
*/

#ifndef MYREDIS_H_
#define MYREDIS_H_

#include <iconv.h>
#include <string>
#include <stdlib.h>
#include "hiredis/hiredis.h"

using std::string;

#define REDIS_CMD_STRING	1
#define REDIS_CMD_INT		2

class MyRedis
{
public:
	MyRedis();
	~MyRedis();

	bool DeInit();

	//连接数据库
	bool ConnectDB(string strIP, int nPort, string strPasswd);
	bool ReConnectDB();

	//验证数据库
	bool Auth(string &strPasswd);

	void SetListMaxNum(int nMaxNum) 
	{
		m_nRedisListMaxNum = nMaxNum;
		printf("Set Redis List Max Num:%d\n", m_nRedisListMaxNum);
	}

public:
	//多个参数 手动执行这里的命令
	/*
		注意：当前只有 **Command1支持GB2312到Utf-8格式的转换
	*/

	//执行Get Set命令(HiRedis)
	bool ExecSetCommand(int nType, const char *format, ...);
	bool ExecGetCommand(int nType, string &strResult, const char *format, ...);

	//简单 Set Get
	//string
	bool SetCommand(string &strKey, string &strValue);
    bool SetCommand(const char *pKey, const char *strValue);
	bool GetCommand(string &strKey, string &strResult);

	//hash
	bool HSetCommand(string &strKey, string &strField, string &strValue);
	bool HSetCommand(const char *pKey, const char *pField, char *pValue, bool bConvertEncoding = false);
	bool HGetCommand(string &strKey, string &strField, string &strResult);
	bool HGetCommand(const char *pKey, const char *pField, string &strResult);

	//List Push Pop(右边压入 左边弹出)
	bool RPUSHCommand(string &strKey, string &strValue);
	bool RPUSHCommand(const char *pKey, char *pValue, bool bConvertEncoding = false);
	bool LPOPCommand(string &strKey, string &strResult);
	bool LPOPCommand(const char *pKey, string &strResult);

	int  LLENCommand(string &strKey);
	int  LLENCommand(const char *pKey);

public:
	bool IsConnect() { return m_bConnect; }

	//编码格式转换
	//将 pValue中的 GB2312编码字符转换为 utf-8,存储到 m_pEncodingConvertBuf中
	bool ConvertEncoding(char *pInBuf, size_t nInLen);

private:
	bool			m_bConnect;
	int				m_nRedisListMaxNum;	//Redis数据库中元素最大值
	
	string			m_strDBIP;
	int				m_nDBPort;
	string			m_strPasswd;

	redisContext	*m_pRedis;
	int				m_nListCurNum;		//Redis数据库中 List当前大小

private:
	iconv_t			m_EncodingConvert;
	char			*m_pEncodingConvertBuf;
	size_t			m_nEncodingConvertBufSize;
};

#endif 

