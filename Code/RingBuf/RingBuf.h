#ifndef _TCP_RING_H_
#define _TCP_RING_H_


#define MAX_RING_SIZE		1024 * 1024 * 500


class CRingBuf
{
public:
	CRingBuf();
	~CRingBuf();

public:
	//初始化
	bool Init(unsigned int nSize = MAX_RING_SIZE);

    void Clear();

	//写入数据
	char *Write(char *pData, unsigned int nLen);

	//设置读位置
	bool SetRead(char *pData);

	float GetUsed(unsigned int& uDrop);

public:
	char *m_pData;

	unsigned int m_nWrPos;		//写指针
	unsigned int m_nRdPos;		//读指针

	unsigned int m_nSize;

	unsigned int m_uDropCount;
	
};
#endif

