#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "RingBuf.h"


CRingBuf::CRingBuf()
{
	m_pData = NULL;
	m_nSize = 0;
	m_nWrPos = 0;
	m_nRdPos = 0;
	m_uDropCount = 0;
}

CRingBuf::~CRingBuf()
{
	if (m_pData)
	{
		free(m_pData);
	}	
}

bool CRingBuf::Init(unsigned int nSize)
{
	if(NULL != m_pData)
	{
		return false;
	}

	m_pData = (char *)malloc(nSize);
	if(NULL == m_pData)
	{
		printf("CRingBuf::Init malloc failed %u Bytes\n", nSize);
		return false;
	}

	m_nSize = nSize;
	m_nWrPos = 0;
	m_nRdPos = 0;

	return true;
}

void CRingBuf::Clear()
{
    m_nWrPos = 0;
    m_nRdPos = 0;
}

char* CRingBuf::Write(char* pData,unsigned int nLen)
{
	char *pRet = NULL;
	if(NULL == pData || 0 == nLen)
		return pRet;

	unsigned int nWr = m_nWrPos;
	unsigned int nRd = m_nRdPos;
	unsigned int nTmpLen = nLen + 1;		//最后添加一个'\0'结束符
	if(m_nSize - nWr > nTmpLen)
	{
		if((nWr >= nRd) || (nRd - nWr > nTmpLen))
		{
			pRet = m_pData + nWr;
			memcpy(pRet, pData, nLen);
			pRet[nLen] = '\0';
			nWr += nTmpLen;
		}
	}
	else
	{
		if((nRd > nTmpLen) && (nWr > nRd))
		{
			pRet = m_pData;
			memcpy(pRet, pData, nLen);
			pRet[nLen] = '\0';
			nWr = nTmpLen;
		}
	}

	m_nWrPos = nWr;

	if (NULL == pRet)
	{
		++m_uDropCount;
		printf("CRingBuf::Write() err: no buf for fill data <Wr:%d, Rd:%d, InputSize=%d>\n", m_nWrPos, m_nRdPos, nLen);
	}

	return pRet;
}

bool CRingBuf::SetRead(char* pData)
{
	if (NULL == pData)
	{
		return false;
	}

	if((pData < m_pData) || (pData > m_pData + m_nSize))
	{
		//printf("CTcpRing SetRead Error<0x%08x, 0x%08x, %u>!\n", pData, m_pData, m_nSize);
		return false;
	}

	m_nRdPos = (unsigned int)(pData - m_pData);

	return true;
}

float CRingBuf::GetUsed(unsigned int& uDrop)
{
	uDrop = m_uDropCount;

	if (0 == m_nSize)
	{
		return 0.0;
	}

	return (float)((m_nWrPos - m_nRdPos + m_nSize) % m_nSize) / (float)m_nSize;
}

