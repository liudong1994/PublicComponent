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
	*   @brief       Connect            连接rabbitmq服务器并创建一个channel
	*	@param       [in]               vecAddrs        服务器集群地址组
	*   @param       [in]               strUser         用户名
    *   @param       [in]               strPasswd       密码
    *   @param       [in]               timeout         连接服务器超时时间
	*   @return 等于0值代表成功连接服务器成功，小于0代表错误
	*/
    int Connect(const vector<pair<string, int>> &vecAddrs, const string &strUser, const string &strPasswd, int iRetry, timeval *timeout = nullptr);

    int Disconnect();

    int ReConnect();


    /**
	*   @brief       ExchangeDeclare    声明exchange
	*	@param       [in]               strExchange    
	*   @param       [in]               strType
	*   @return 等于0值代表成功创建exchange，小于0代表错误
	*/
    int ExchangeDeclare(const string &strExchange, const string &strType);

    /**
	*   @brief       QueueDeclare                     声明消息队列
	*	@param       [in]               strQueueName  消息队列实例
	*   @param       
	*   @return 等于0值代表成功创建queue，小于0代表错误
	*/
    int QueueDeclare(const string &strQueueName);

    /**
	*   @brief       QueueBind                      将队列，交换机和绑定规则绑定起来形成一个路由表
	*	@param       [in]           strQueueName    消息队列
	*	@param       [in]           strExchange     交换机名称
	*	@param       [in]           strBindKey      路由名称
    *   @return 等于0值代表成功绑定，小于0代表错误
	*/
    int QueueBind(const string &strQueueName, const string &strExchange, const string &strBindKey);

    /**
	*   @brief       QueueUnbind                      将队列，交换机和绑定规则绑定解除
	*	@param       [in]               strQueueName  消息队列
	*	@param       [in]               strExchange   交换机名称
	*	@param       [in]               strBindKey    路由名称
    *   @return 等于0值代表成功绑定，小于0代表错误
	*/
    int QueueUnbind(const string &strQueueName, const string &strExchange, const string &strBindKey);

    /**
	*   @brief       QueueDelete                    删除消息队列。
	*	@param       [in]           strQueueName    消息队列名称
	*	@param       [in]           iIfUnused       消息队列是否在用，1 则论是否在用都删除
	*   @return 等于0值代表成功删除queue，小于0代表错误
	*/
    int QueueDelete(const string &strQueueName, int iIfUnused);

    /**
	* @brief Publish  发布消息
	* @param [in]   strMessage        消息实体
    * @param [in]   strExchange       交换器
	* @param [in]   strRoutekey       路由规则 
	* @return 等于0值代表成功发送消息实体，小于0代表发送错误
	*/
    int Publish(const string &strMessage, const string &strExchange, const string &strRoutekey);

    /** 
	* @brief ConsumerNeedAck  消费一条消息 需要确认
	* @param [in]   strQueueName        队列名称
	* @param [out]  strMessage          获取的消息实体
    * @param [in]   ullAckTag           确认消息时需要的tag
	* @param [in]   timeout             取得的消息是延迟，若为NULL，表示持续取，无延迟，阻塞状态
	* @return 等于0值代表成功，小于0代表错误，错误信息从ErrorReturn返回
    * @warn  调用完成后 需要调用ConsumeAck(...) 确认以及释放资源
	*/
    int ConsumeNeedAck(const string &strQueueName, string &strMessage, uint64_t &ullAckTag, struct timeval *timeout = NULL);

    /** 
	* @brief ConsumeAck  确认消息
	* @param [in]       ullAckTag       确认消息时需要的tag
    * @param [in]       iMultiple       1-确认此tag之前的所有数据  0-只确认此tag
	* @return 等于0值代表成功，小于0代表错误，错误信息从ErrorReturn返回
	*/
    int ConsumeAck(uint64_t ullAckTag, int iMultiple = 0);


    /** 
	* @brief Consumer  消费GetNum个消息
	* @param [in]   strQueueName         队列名称
	* @param [out]  vecMessage           获取的消息实体数组
    * @param [in]   iGetNum              需要取得的消息个数
	* @param [in]   timeout              取得的消息是延迟，若为NULL，表示持续取，无延迟，阻塞状态
	* @return 等于0值代表成功，小于0代表错误，错误信息从ErrorReturn返回
	*/
    int Consume(const string &strQueueName, vector<string> &vecMessage, int iGetNum = 1, struct timeval *timeout = NULL);


private:
    CRabbitmqClient(const CRabbitmqClient & rh);
    void operator=(const CRabbitmqClient & rh);

    int ErrorMsg(amqp_rpc_reply_t x, char const *context);


private:
    vector<pair<string, int>>   m_vecAddrs;         // amqp地址组(集群的多个地址)
    int                         m_iAddrsIndex;      // amqp地址组使用索引

    string					    m_strUser;
    string					    m_strPasswd;
    int                         m_iChannel;
    int                         m_iRetry;

    struct timeval              m_timeout;


private:
    amqp_socket_t               *m_pSock;        
    amqp_connection_state_t     m_pConn;

    bool                        m_bBasicConsume;    // 是否打开basicConsume
};

#endif

