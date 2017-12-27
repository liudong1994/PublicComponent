#include "MyRedis.h"
#include <string.h>

#define REDIS_LIST_MAX_NUM	1000
#define ENCODING_CONVERT_BUF_SIZE	10240

#define ENCODING_GB2312	"gb2312"
#define ENCODING_UTF_8	"utf-8"


MyRedis::MyRedis()
{
	m_bConnect = false;
	m_nRedisListMaxNum = REDIS_LIST_MAX_NUM;
	m_pRedis = NULL;
	m_nListCurNum = 0;
	m_EncodingConvert = (iconv_t)-1;

	m_pEncodingConvertBuf = NULL;
	m_nEncodingConvertBufSize = ENCODING_CONVERT_BUF_SIZE;
}

MyRedis::~MyRedis()
{
	DeInit();
}

bool MyRedis::DeInit()
{
	m_bConnect = false;

	if(m_pRedis)
	{
		redisFree(m_pRedis);
		m_pRedis = NULL;
	}
	
	if(m_EncodingConvert != (iconv_t) -1)
	{
		iconv_close(m_EncodingConvert);
	}

	if(m_pEncodingConvertBuf != NULL)
	{
		delete m_pEncodingConvertBuf;
        m_pEncodingConvertBuf = NULL;
	}

	return true;
}

bool MyRedis::ConnectDB(string strIP, int nPort, string strPasswd)
{
	//初始化 编码格式转换
	m_EncodingConvert = iconv_open(ENCODING_UTF_8, ENCODING_GB2312);
	if(m_EncodingConvert == (iconv_t) -1)
	{
		printf("iconv_open Failed\n");
		return false;
	}

	m_pEncodingConvertBuf = (char *)malloc(m_nEncodingConvertBufSize);
	if(m_pEncodingConvertBuf == NULL)
	{
		printf("malloc EncodingConvertBuf Failed\n");
		return false;
	}

	m_strDBIP = strIP;
	m_nDBPort = nPort;
	m_strPasswd = strPasswd;

	printf("Connect to Redis(%s:%d)...\n", m_strDBIP.c_str(), m_nDBPort);
	m_pRedis = redisConnect(m_strDBIP.c_str(), m_nDBPort);

	if(!m_pRedis)
	{
		printf("Connect to Redis Failed...\n");
		return false;
	}

	if(m_pRedis->err)
	{
		printf("Connect to Redis Error: %d %s\n", m_pRedis->err, m_pRedis->errstr);
		return false;
	} 

	if(strPasswd != "" && !Auth(strPasswd))
	{
		return false;
	}

	m_bConnect = true;
	printf("Connect to Redis(%s:%d) Success\n", m_strDBIP.c_str(), m_nDBPort);

	return true;
}

bool MyRedis::Auth(string &strPasswd)
{
	redisReply *pRedisReply  = (redisReply*)redisCommand(m_pRedis, "AUTH %s", strPasswd.c_str()); 

	if(strcmp(pRedisReply->str, "OK") != 0)
	{
		printf("Redis Auth Failed, Desc:%s\n", pRedisReply->str);
		return false;
	}
	freeReplyObject(pRedisReply);

	printf("Redis Auth Success\n");

	return true;
}

bool MyRedis::ReConnectDB()
{
	DeInit();

	return ConnectDB(m_strDBIP, m_nDBPort, m_strPasswd);
}

bool MyRedis::ExecSetCommand(int nType, const char *format, ...)
{
	if(!m_bConnect)
		return false;

	va_list ap;
	va_start(ap,format);
	
	redisReply *pRedisReply  = (redisReply*)redisvCommand(m_pRedis, format, ap); 
	va_end(ap);

	if(pRedisReply)
	{
		if(nType == REDIS_CMD_INT)
		{
			m_nListCurNum = pRedisReply->integer;
		}
		else if(nType == REDIS_CMD_STRING)
		{
			if(pRedisReply->str && strcmp(pRedisReply->str, "OK") != 0)
			{
				printf("Exec Set Failed, Desc:%s\n", pRedisReply->str);
			}
		}

		freeReplyObject(pRedisReply);
	}
	else
	{
		m_bConnect = false;
	}

	return true;
}

bool MyRedis::ExecGetCommand(int nType, string &strResult, const char *format, ...)
{
	if(!m_bConnect)
		return false;

	va_list ap;
	va_start(ap,format);

	redisReply *pRedisReply  = (redisReply*)redisvCommand(m_pRedis, format, ap); 
	va_end(ap);

	if(pRedisReply)
	{
		if(nType == REDIS_CMD_INT)
		{
			m_nListCurNum = pRedisReply->integer;
		}
		else if(nType == REDIS_CMD_STRING)
		{
			if(pRedisReply->str)
			{
				strResult = pRedisReply->str;
			}
			else
			{
				strResult = "";
			}
		}

		freeReplyObject(pRedisReply);
	}
	else
	{
		m_bConnect = false;
	}

	return true;
}

bool MyRedis::SetCommand(string &strKey, string &strValue)
{
	return ExecSetCommand(REDIS_CMD_STRING, "SET %s %s", strKey.c_str(), strValue.c_str());
}

bool MyRedis::SetCommand(const char *pKey, const char *strValue)
{
    return ExecSetCommand(REDIS_CMD_STRING, "SET %s %s", pKey, strValue);
}

bool MyRedis::GetCommand(string &strKey, string &strResult)
{
	return ExecGetCommand(REDIS_CMD_STRING, strResult, "GET %s", strKey.c_str());
}

bool MyRedis::HSetCommand(string &strKey, string &strField, string &strValue)
{
	return ExecSetCommand(REDIS_CMD_STRING, "HSET %s %s %s", strKey.c_str(), strField.c_str(), strValue.c_str());
}

bool MyRedis::HSetCommand(const char *pKey, const char *pField, char *pValue, bool bConvertEncoding)
{
    if(bConvertEncoding)
    {
	    if(!ConvertEncoding(pValue, strlen(pValue)))
		    return false;

        return ExecSetCommand(REDIS_CMD_STRING, "HSET %s %s %s", pKey, pField, m_pEncodingConvertBuf);
    }
    
    return ExecSetCommand(REDIS_CMD_STRING, "HSET %s %s %s", pKey, pField, pValue);
}

bool MyRedis::HGetCommand(string &strKey, string &strField, string &strResult)
{
	return ExecGetCommand(REDIS_CMD_STRING, strResult, "HGET %s %s", strKey.c_str(), strField.c_str());
}

bool MyRedis::HGetCommand(const char *pKey, const char *pField, string &strResult)
{
	return ExecGetCommand(REDIS_CMD_STRING, strResult, "HGET %s %s", pKey, pField);
}

bool MyRedis::RPUSHCommand(string &strKey, string &strValue)
{
	if(m_nListCurNum >= m_nRedisListMaxNum)
	{
		//再次获取Redis List当前数量(即时更新数据库list当前数量)
		if(LLENCommand(strKey.c_str()) >= m_nRedisListMaxNum)
		{
			printf("Redis List Num:%d >= Max:%d\n", m_nListCurNum, m_nRedisListMaxNum);
			return false;
		}
	}

	if(!ExecSetCommand(REDIS_CMD_INT, "RPUSH %s %s", strKey.c_str(), strValue.c_str()))
		return false;

	return true;
}

bool MyRedis::RPUSHCommand(const char *pKey, char *pValue, bool bConvertEncoding)
{
	if(m_nListCurNum >= m_nRedisListMaxNum)
	{
		//再次获取Redis List当前数量(即时更新数据库list当前数量)
		if(LLENCommand(pKey) >= m_nRedisListMaxNum)
		{
			printf("Redis List Num:%d >= Max:%d\n", m_nListCurNum, m_nRedisListMaxNum);
			return false;
		}
	}

    if(bConvertEncoding)
    {
	    if(!ConvertEncoding(pValue, strlen(pValue)))
		    return false;

        if(!ExecSetCommand(REDIS_CMD_INT, "RPUSH %s %s", pKey, m_pEncodingConvertBuf))
            return false;

        return true;
    }

    if(!ExecSetCommand(REDIS_CMD_INT, "RPUSH %s %s",pKey, pValue))
        return false;

	return true;
}

bool MyRedis::LPOPCommand(string &strKey, string &strResult)
{
	return ExecGetCommand(REDIS_CMD_STRING, strResult, "LPOP %s", strKey.c_str());
}

bool MyRedis::LPOPCommand(const char *pKey, string &strResult)
{
	return ExecGetCommand(REDIS_CMD_STRING, strResult, "LPOP %s", pKey);
}

int  MyRedis::LLENCommand(string &strKey)
{
	string strTmp;
	if(!ExecGetCommand(REDIS_CMD_INT, strTmp, "LLEN %s", strKey.c_str()))
		return m_nRedisListMaxNum;

	printf("Redis Cur Len : %d\n", m_nListCurNum);

	return m_nListCurNum;
}

int  MyRedis::LLENCommand(const char *pKey)
{
	string strTmp;
	if(!ExecGetCommand(REDIS_CMD_INT, strTmp, "LLEN %s", pKey))
		return m_nRedisListMaxNum;

	printf("Redis Cur Len : %d\n", m_nListCurNum);

	return m_nListCurNum;
}

bool MyRedis::ConvertEncoding(char *pInBuf, size_t nInLen)
{
	//buf内存不够
	//GB2312中文编码一般占用2个字节  utf-8占用3个字节(极个别占用4个字节)
	while((nInLen*2) > m_nEncodingConvertBufSize)
	{
		m_nEncodingConvertBufSize *= 2;
		printf("ConvertEncoding Buf no enough, now:%ld nLen:%ld\n", m_nEncodingConvertBufSize, nInLen);

		delete m_pEncodingConvertBuf;
        m_pEncodingConvertBuf = NULL;

		m_pEncodingConvertBuf = (char *)malloc(m_nEncodingConvertBufSize);
		if(!m_pEncodingConvertBuf)
		{
			printf("ConverEncoding Buf Malloc Failed\n");
			exit(0);
		}
	}

	char **ppIn = &pInBuf;
	char *pBufTmp = m_pEncodingConvertBuf;
	char **ppOut = &pBufTmp;
	//需要使用临时变量 iconv会改变参数的值
	size_t nEncodingConvertBufSize = m_nEncodingConvertBufSize;
	memset(m_pEncodingConvertBuf, 0, m_nEncodingConvertBufSize);

	//iconv会修改 ppOut指针,这里传入 pBufTmp的地址,否则 m_pEncodingConverBuf地址会被修改
	if (iconv(m_EncodingConvert, ppIn, &nInLen, ppOut, &nEncodingConvertBufSize) == (unsigned int)-1)
	{
		printf("ConvertEncoding Failed, In:%s Out:%s\n", pInBuf, m_pEncodingConvertBuf);
		return false;
	}

	return true;
}

