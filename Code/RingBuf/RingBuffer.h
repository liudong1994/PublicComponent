#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

#include <vector>
#include <string>
#include <stdio.h>

#ifdef LogN
#define Log	LogN(80)
#else
#define Log	printf
#endif

#define DEFAULT_BUFFER_SIZE		1000

template<class T, int nSize>
class CRingBuffer
{
public:
	CRingBuffer()
	{ 
		if (nSize <=3)
		{
			Log("RingBuffer nSize(%d) Error\n", nSize);
		}

		m_strBufName = "";
		m_nBufSize = nSize; 
		m_pBuf.resize(nSize);

		m_nWrite = 1;
		m_nRead = 0;
	};

	virtual ~CRingBuffer()
	{ 
	};


public:
	int GetReadPos()
	{
		return m_nRead;
	}

	unsigned int Size()
	{
		if (m_nRead < m_nWrite)
		{
			return m_nWrite - m_nRead - 1;
		}
		else
		{
			return nSize - m_nRead - 1 + m_nWrite;
		}
	};

	void SetBufferName(const char * strBufName)
	{
		m_strBufName = strBufName;
	}

	bool IsEmpty()
	{
		//计算下一个读位置
		int nRTemp = (m_nRead + 1) % m_nBufSize;

		if (nRTemp == m_nWrite)
		{
			return true;
		}

		return false;
	};

    bool GetData(T& nd)
    {
        //计算下一个读位置
        int nRTemp = (m_nRead + 1) % m_nBufSize;
        if (nRTemp == m_nWrite)
        {
            return false;
        }

        nd = m_pBuf[nRTemp];

        return true;
    }

    void PopData()
    {
        //计算下一个读位置
        int nRTemp = (m_nRead + 1) % m_nBufSize;
        if (nRTemp == m_nWrite)
        {
            return;
        }

        m_nRead = nRTemp;

        return;
    }

	T Pop()
	{
		//计算下一个读位置
		int nRTemp = (m_nRead + 1) % m_nBufSize;

		T nd;
		if (nRTemp != m_nWrite)
		{
			nd = m_pBuf[nRTemp];
			m_nRead = nRTemp;
		}

		return nd;
	};

	bool Pop(T& nd)
	{
		//计算下一个读位置
		int nRTemp = (m_nRead + 1) % m_nBufSize;
		if (nRTemp == m_nWrite)
		{
			return false;
		}

		nd = m_pBuf[nRTemp];
		m_nRead = nRTemp;		

		return true;
	};

	bool Push(const T& node)
	{
		//计算下一个写位置
		int nWTemp = (m_nWrite + 1) % m_nBufSize;

		if (nWTemp != m_nRead)
		{
			m_pBuf[m_nWrite] = node;
			m_nWrite = nWTemp;
			return true;
		}
		else
		{
			Log("Read POS = %d , Write Pos = %d RingBuffer(%s) Err: can't write\n", m_nRead, m_nWrite, m_strBufName.c_str());
			return false;
		}
	};

	void Clear()
	{
        m_nWrite = 1;
        m_nRead = 0;
	};

	//取出指定位置的数据
	T& operator[] (unsigned int nPos)
	{
		if (nPos + 1 > Size())
		{
			T* nd = NULL;
			return *nd;
		}

		//计算读位置
		int nRTemp = (m_nRead + nPos + 1) % m_nBufSize;

		return m_pBuf[nRTemp];
	};

private:
	int m_nBufSize;
	std::vector<T> m_pBuf;

	int m_nWrite;
	int m_nRead;

	std::string m_strBufName;
};
#endif

