#ifndef RABBITMQ_CLIENT_H_
#define RABBITMQ_CLIENT_H_


#include <string>
#include <vector>
#include <thread>
#include "amqp_tcp_socket.h"

using std::string;
using std::vector;
using std::pair;


class CRabbitmqClient {
public:
    explicit CRabbitmqClient(int iChannle = 1);
    ~CRabbitmqClient();

    /**
	*   @brief       Connect            ����rabbitmq������������һ��channel
	*	@param       [in]               vecAddrs        ��������Ⱥ��ַ��
	*   @param       [in]               strUser         �û���
    *   @param       [in]               strPasswd       ����
    *   @param       [in]               timeout         ���ӷ�������ʱʱ��
	*   @return ����0ֵ����ɹ����ӷ������ɹ���С��0�������
	*/
    int Connect(const vector<pair<string, int>> &vecAddrs, const string &strUser, const string &strPasswd, timeval *timeout = nullptr);

    int Disconnect();

    int ReConnect();


    /**
	*   @brief       ExchangeDeclare    ����exchange
	*	@param       [in]               strExchange    
	*   @param       [in]               strType
	*   @return ����0ֵ����ɹ�����exchange��С��0�������
	*/
    int ExchangeDeclare(const string &strExchange, const string &strType);

    /**
	*   @brief       QueueDeclare                     ������Ϣ����
	*	@param       [in]               strQueueName  ��Ϣ����ʵ��
	*   @param       
	*   @return ����0ֵ����ɹ�����queue��С��0�������
	*/
    int QueueDeclare(const string &strQueueName);

    /**
	*   @brief       QueueBind                      �����У��������Ͱ󶨹���������γ�һ��·�ɱ�
	*	@param       [in]           strQueueName    ��Ϣ����
	*	@param       [in]           strExchange     ����������
	*	@param       [in]           strBindKey      ·������
    *   @return ����0ֵ����ɹ��󶨣�С��0�������
	*/
    int QueueBind(const string &strQueueName, const string &strExchange, const string &strBindKey);

    /**
	*   @brief       QueueUnbind                      �����У��������Ͱ󶨹���󶨽��
	*	@param       [in]               strQueueName  ��Ϣ����
	*	@param       [in]               strExchange   ����������
	*	@param       [in]               strBindKey    ·������
    *   @return ����0ֵ����ɹ��󶨣�С��0�������
	*/
    int QueueUnbind(const string &strQueueName, const string &strExchange, const string &strBindKey);

    /**
	*   @brief       QueueDelete                    ɾ����Ϣ���С�
	*	@param       [in]           strQueueName    ��Ϣ��������
	*	@param       [in]           iIfUnused       ��Ϣ�����Ƿ����ã�1 �����Ƿ����ö�ɾ��
	*   @return ����0ֵ����ɹ�ɾ��queue��С��0�������
	*/
    int QueueDelete(const string &strQueueName, int iIfUnused);

    /**
	* @brief Publish  ������Ϣ
	* @param [in]   strMessage        ��Ϣʵ��
    * @param [in]   strExchange       ������
	* @param [in]   strRoutekey       ·�ɹ��� 
    *   1.Direct Exchange �C ����·�ɼ�����Ҫ��һ�����а󶨵��������ϣ�Ҫ�����Ϣ��һ���ض���·�ɼ���ȫƥ�䡣
    *   2.Fanout Exchange �C ������·�ɼ��������а󶨵��������ϡ�һ�����͵�����������Ϣ���ᱻת������ý������󶨵����ж����ϡ�
	*   3.Topic  Exchange �C ��·�ɼ���ĳģʽ����ƥ�䡣��ʱ������Ҫ��Ҫһ��ģʽ�ϡ�����"#"ƥ��һ�������ʣ�����"*"ƥ�䲻�಻��һ���ʡ�
    *      ���"audit.#"�ܹ�ƥ�䵽"audit.irs.corporate"������"audit.*" ֻ��ƥ�䵽"audit.irs"
	* @return ����0ֵ����ɹ�������Ϣʵ�壬С��0�����ʹ���
	*/
    int Publish(const string &strMessage, const string &strExchange, const string &strRoutekey);

    /** 
	* @brief ConsumerNeedAck  ����һ����Ϣ ��Ҫȷ��
	* @param [in]   strQueueName        ��������
	* @param [out]  strMessage          ��ȡ����Ϣʵ��
    * @param [in]   ullAckTag           ȷ����Ϣʱ��Ҫ��tag
	* @param [in]   timeout             ȡ�õ���Ϣ���ӳ٣���ΪNULL����ʾ����ȡ�����ӳ٣�����״̬
	* @return ����0ֵ����ɹ���С��0������󣬴�����Ϣ��ErrorReturn����
    * @warn  ������ɺ� ��Ҫ����ConsumeAck(...) ȷ���Լ��ͷ���Դ
	*/
    int ConsumeNeedAck(const string &strQueueName, string &strMessage, uint64_t &ullAckTag, struct timeval *timeout = NULL);

    /** 
	* @brief ConsumeAck  ȷ����Ϣ
	* @param [in]       ullAckTag       ȷ����Ϣʱ��Ҫ��tag
    * @param [in]       iMultiple       1-ȷ�ϴ�tag֮ǰ����������  0-ֻȷ�ϴ�tag
	* @return ����0ֵ����ɹ���С��0������󣬴�����Ϣ��ErrorReturn����
	*/
    int ConsumeAck(uint64_t ullAckTag, int iMultiple = 0);


    /** 
	* @brief Consumer  ����GetNum����Ϣ
	* @param [in]   strQueueName         ��������
	* @param [out]  vecMessage           ��ȡ����Ϣʵ������
    * @param [in]   iGetNum              ��Ҫȡ�õ���Ϣ����
	* @param [in]   timeout              ȡ�õ���Ϣ���ӳ٣���ΪNULL����ʾ����ȡ�����ӳ٣�����״̬
	* @return ����0ֵ����ɹ���С��0������󣬴�����Ϣ��ErrorReturn����
	*/
    int Consume(const string &strQueueName, vector<string> &vecMessage, int iGetNum = 1, struct timeval *timeout = NULL);


private:
    CRabbitmqClient(const CRabbitmqClient & rh);
    void operator=(const CRabbitmqClient & rh);

    int ErrorMsg(amqp_rpc_reply_t x, char const *context);


private:
    vector<pair<string, int>>   m_vecAddrs;         // amqp��ַ��(��Ⱥ�Ķ����ַ)
    int                         m_iAddrsIndex;      // amqp��ַ��ʹ������

    string					    m_strUser;
    string					    m_strPasswd;
    int                         m_iChannel;
    int                         m_iRetry;

    struct timeval              m_timeout;


private:
    amqp_socket_t               *m_pSock;        
    amqp_connection_state_t     m_pConn;

    bool                        m_bBasicConsume;    // �Ƿ��basicConsume
};

#endif

