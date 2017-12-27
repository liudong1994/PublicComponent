/*
	Redis���ݿ�ӿ�
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

	//�������ݿ�
	bool ConnectDB(string strIP, int nPort, string strPasswd);
	bool ReConnectDB();

	//��֤���ݿ�
	bool Auth(string &strPasswd);

	void SetListMaxNum(int nMaxNum) 
	{
		m_nRedisListMaxNum = nMaxNum;
		printf("Set Redis List Max Num:%d\n", m_nRedisListMaxNum);
	}

public:
	//������� �ֶ�ִ�����������
	/*
		ע�⣺��ǰֻ�� **Command1֧��GB2312��Utf-8��ʽ��ת��
	*/

	//ִ��Get Set����(HiRedis)
	bool ExecSetCommand(int nType, const char *format, ...);
	bool ExecGetCommand(int nType, string &strResult, const char *format, ...);

	//�� Set Get
	//string
	bool SetCommand(string &strKey, string &strValue);
    bool SetCommand(const char *pKey, const char *strValue);
	bool GetCommand(string &strKey, string &strResult);

	//hash
	bool HSetCommand(string &strKey, string &strField, string &strValue);
	bool HSetCommand(const char *pKey, const char *pField, char *pValue, bool bConvertEncoding = false);
	bool HGetCommand(string &strKey, string &strField, string &strResult);
	bool HGetCommand(const char *pKey, const char *pField, string &strResult);

	//List Push Pop(�ұ�ѹ�� ��ߵ���)
	bool RPUSHCommand(string &strKey, string &strValue);
	bool RPUSHCommand(const char *pKey, char *pValue, bool bConvertEncoding = false);
	bool LPOPCommand(string &strKey, string &strResult);
	bool LPOPCommand(const char *pKey, string &strResult);

	int  LLENCommand(string &strKey);
	int  LLENCommand(const char *pKey);

public:
	bool IsConnect() { return m_bConnect; }

	//�����ʽת��
	//�� pValue�е� GB2312�����ַ�ת��Ϊ utf-8,�洢�� m_pEncodingConvertBuf��
	bool ConvertEncoding(char *pInBuf, size_t nInLen);

private:
	bool			m_bConnect;
	int				m_nRedisListMaxNum;	//Redis���ݿ���Ԫ�����ֵ
	
	string			m_strDBIP;
	int				m_nDBPort;
	string			m_strPasswd;

	redisContext	*m_pRedis;
	int				m_nListCurNum;		//Redis���ݿ��� List��ǰ��С

private:
	iconv_t			m_EncodingConvert;
	char			*m_pEncodingConvertBuf;
	size_t			m_nEncodingConvertBufSize;
};

#endif 

