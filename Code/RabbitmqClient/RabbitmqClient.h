#ifndef RABBITMQ_CLIENT_H_
#define RABBITMQ_CLIENT_H_


#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <thread>
#include "amqp_tcp_socket.h"


using namespace std;


class CRabbitmqClient {
public:
    static int getInstance(const int iModel, shared_ptr<CRabbitmqClient> &pDoSql);
    static int Init(const int iModel, const vector<pair<string, int>> &vecAddrs, const string &strUser, const string &strPasswd, int iRetry, int iHeartbeat = 0);

    ~CRabbitmqClient();

    /**
	*   @brief       Connect            ����rabbitmq������������һ��channel
	*	@param       [in]               vecAddrs        ��������Ⱥ��ַ��
	*   @param       [in]               strUser         �û���
    *   @param       [in]               strPasswd       ����
    *   @param       [in]               iRetry          ������Ϣʱ������Դ���
    *   @param       [in]               iHeartbeat      ����ʱ�䣨����ǽ�����ϢMQ������ô�ʱ��  ������ϢMQ����Ϊ0����(������Ϣʱ�߳��������� MQ��������,������ϢMQû���̴߳���������)��
    *   @param       [in]               timeout         ���ӷ�������ʱʱ��
	*   @return ����0ֵ����ɹ����ӷ������ɹ���С��0�������
	*/
    int Connect(const vector<pair<string, int>> &vecAddrs, const string &strUser, const string &strPasswd, int iRetry, int iHeartbeat = 0, timeval *timeout = nullptr);

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
	* @brief ConsumeNeedAck  ����һ����Ϣ ��Ҫȷ��
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
	* @brief Consume  ����GetNum����Ϣ
	* @param [in]   strQueueName         ��������
	* @param [out]  vecMessage           ��ȡ����Ϣʵ������
    * @param [in]   iGetNum              ��Ҫȡ�õ���Ϣ����
	* @param [in]   timeout              ȡ�õ���Ϣ���ӳ٣���ΪNULL����ʾ����ȡ�����ӳ٣�����״̬
	* @return ����0ֵ����ɹ���С��0������󣬴�����Ϣ��ErrorReturn����
	*/
    int Consume(const string &strQueueName, vector<string> &vecMessage, int iGetNum = 1, struct timeval *timeout = NULL);

    /** 
	* @brief SendHeartbeats  ��������������
	* @return ����0ֵ����ɹ���С��0�������
	*/
    int SendHeartbeats();

private:
    explicit CRabbitmqClient(int iChannle = 1);
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
    int                         m_iHeartbeat;

    struct timeval              m_timeout;


private:
    amqp_socket_t               *m_pSock;        
    amqp_connection_state_t     m_pConn;

    bool                        m_bBasicConsume;    // �Ƿ��basicConsume
    std::mutex                  m_mtxSendMsg;       // ��֤������������ͨ��Ϣ����ͻ��rabbitmq-c���̰߳�ȫ��

    // Proxy
private:
    static std::mutex m_mtxProxy;
    static std::unordered_map<int, const shared_ptr<CRabbitmqClient>> m_hProxyRabbitmq;
};

#endif

