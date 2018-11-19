#include "RabbitmqClient.h"
#include <unistd.h>

const int AMQP_OK = 0;
const int AMQP_ERROR = -1;
const int AMQP_TIMEOUT = -2;



CRabbitmqClient::CRabbitmqClient(int iChannle)
: m_iAddrsIndex(0)
, m_strUser("")
, m_strPasswd("")
, m_iChannel(iChannle)
, m_iRetry(3)
, m_pSock(NULL)
, m_pConn(NULL)
, m_bBasicConsume(false) {

}

CRabbitmqClient::~CRabbitmqClient() {
    m_bBasicConsume = false;

    if (NULL != m_pConn) {
        Disconnect();
        m_pConn = NULL;
    }
}

int CRabbitmqClient::Connect(const vector<pair<string, int>> &vecAddrs, const string &strUser, const string &strPasswd, int iRetry, timeval *timeout) {
    if (vecAddrs.size() == 0) {
        fprintf(stderr, "amqp addrs size 0");
        return AMQP_ERROR;
    }
    m_vecAddrs = vecAddrs;
    m_strUser = strUser;
    m_strPasswd = strPasswd;
    m_iRetry = iRetry;

    m_pConn = amqp_new_connection();
    if (NULL == m_pConn) {
        fprintf(stderr, "amqp new connection failed\n");
        return AMQP_ERROR;
    }

    m_pSock = amqp_tcp_socket_new(m_pConn);
    if (NULL == m_pSock) {
        fprintf(stderr, "amqp tcp new socket failed\n");
        return AMQP_ERROR;
    }

    if (timeout == NULL) {
        m_timeout.tv_sec = 1;
        m_timeout.tv_usec = 0;
    } else {
        m_timeout.tv_sec = timeout->tv_sec;
        m_timeout.tv_usec = timeout->tv_usec;
    }

    // connect
    size_t uTryTimes = 0;
    for ( ; uTryTimes < m_vecAddrs.size(); ++uTryTimes) {
        // 在地址组里找到 一个连接的ip和port
        const string &strHost = m_vecAddrs[m_iAddrsIndex].first;
        int iPort = m_vecAddrs[m_iAddrsIndex].second;
        m_iAddrsIndex = (m_iAddrsIndex + 1) % m_vecAddrs.size();

        int status = amqp_socket_open_noblock(m_pSock, strHost.c_str(), iPort, &m_timeout);
        if (status < 0) {
            fprintf(stderr, "amqp socket open failed, addrs:%s:%d\n", strHost.c_str(), iPort);
            continue;
        }

        // amqp_login(amqp_connection_state_t state,char const *vhost, int channel_max, int frame_max, int heartbeat, amqp_sasl_method_enum sasl_method, ..)
        if (0 != ErrorMsg(amqp_login(m_pConn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, m_strUser.c_str(), m_strPasswd.c_str()), "Logging in")) {
            fprintf(stderr, "amqp login failed, addrs:%s:%d user:%s passwd:%s\n", strHost.c_str(), iPort, m_strUser.c_str(), m_strPasswd.c_str());
            continue;
        }

        fprintf(stderr, "amqp connect success, addrs:%s:%d user:%s\n", strHost.c_str(), iPort, m_strUser.c_str());
        break;
    }

    if (uTryTimes >= m_vecAddrs.size()) {
        fprintf(stderr, "amqp connect failed, trytimes: %lu\n", uTryTimes);
        return AMQP_ERROR;
    }
  
    // open channel on socket
    amqp_channel_open(m_pConn, m_iChannel);
    if (0 != ErrorMsg(amqp_get_rpc_reply(m_pConn), "open channel")) {
        fprintf(stderr, "amqp channel open failed\n");
        return AMQP_ERROR;
    }

    fprintf(stderr, "amqp connect success channel:%d\n", m_iChannel);
    return AMQP_OK;
}

int CRabbitmqClient::Disconnect() {
    if (NULL != m_pConn) {
        m_bBasicConsume = false;
        amqp_channel_close(m_pConn, m_iChannel, AMQP_REPLY_SUCCESS);

        if (0 != ErrorMsg(amqp_connection_close(m_pConn, AMQP_REPLY_SUCCESS), "Closing connection"))
            return AMQP_ERROR;

        if (amqp_destroy_connection(m_pConn) < 0)
            return AMQP_ERROR;

        m_pConn = NULL;
    }

    return AMQP_OK;
}

int CRabbitmqClient::ReConnect() {
    Disconnect();
    return Connect(m_vecAddrs, m_strUser, m_strPasswd, m_iRetry, nullptr);
}

int CRabbitmqClient::ExchangeDeclare(const string &strExchange, const string &strType) {
    amqp_bytes_t _exchange = amqp_cstring_bytes(strExchange.c_str());
    amqp_bytes_t _type = amqp_cstring_bytes(strType.c_str());
    int _passive= 0;
    int _durable= 1;      // 交换机是否持久化
    amqp_exchange_declare(m_pConn, m_iChannel, _exchange, _type, _passive, _durable, 0, 0, amqp_empty_table);
    if (0 != ErrorMsg(amqp_get_rpc_reply(m_pConn), "exchange_declare")) {
        return AMQP_ERROR;
    }

    return AMQP_OK;
}

int CRabbitmqClient::QueueDeclare(const string &strQueueName) {
    if(NULL == m_pConn) {
        fprintf(stderr, "QueueDeclare m_pConn is null\n");
        return AMQP_ERROR;
    }

    amqp_bytes_t _queue = amqp_cstring_bytes(strQueueName.c_str());
    int32_t _passive = 0;
    int32_t _durable = 1;           // 队列是否持久化
    int32_t _exclusive = 0;         // 当连接不在时 是否自动删除queue
    int32_t _auto_delete = 0;       // 没有消费者时 是否自动删除queue
    amqp_queue_declare(m_pConn, m_iChannel, _queue, _passive, _durable, _exclusive, _auto_delete, amqp_empty_table);
    if (0 != ErrorMsg(amqp_get_rpc_reply(m_pConn), "queue_declare")) {
        return AMQP_ERROR;
    }

    return AMQP_OK;
}

int CRabbitmqClient::QueueBind(const string &strQueueName, const string &strExchange, const string &strBindKey) {
    if(NULL == m_pConn) {
        fprintf(stderr, "QueueBind m_pConn is null\n");
        return AMQP_ERROR;
    }

    amqp_bytes_t _queue = amqp_cstring_bytes(strQueueName.c_str());
    amqp_bytes_t _exchange = amqp_cstring_bytes(strExchange.c_str());
    amqp_bytes_t _routkey  = amqp_cstring_bytes(strBindKey.c_str());
    amqp_queue_bind(m_pConn, m_iChannel, _queue, _exchange, _routkey, amqp_empty_table);
    if(0 != ErrorMsg(amqp_get_rpc_reply(m_pConn), "queue_bind")) {
        return AMQP_ERROR;
    }

    return AMQP_OK;
}

int CRabbitmqClient::QueueUnbind(const string &strQueueName, const string &strExchange, const string &strBindKey) {
    if(NULL == m_pConn) {
        fprintf(stderr, "QueueUnbind m_pConn is null\n");
        return AMQP_ERROR;
    }

    amqp_bytes_t _queue = amqp_cstring_bytes(strQueueName.c_str());
    amqp_bytes_t _exchange = amqp_cstring_bytes(strExchange.c_str());
    amqp_bytes_t _routkey  = amqp_cstring_bytes(strBindKey.c_str());
    amqp_queue_unbind(m_pConn, m_iChannel, _queue, _exchange, _routkey, amqp_empty_table);
    if(0 != ErrorMsg(amqp_get_rpc_reply(m_pConn), "queue_unbind")) {
        return AMQP_ERROR;
    }

    return AMQP_OK;
}

int CRabbitmqClient::QueueDelete(const string &strQueueName, int iIfUnused) {
    if(NULL == m_pConn) {
        fprintf(stderr, "QueueDelete m_pConn is null\n");
        return AMQP_ERROR;
    }

    amqp_queue_delete(m_pConn, m_iChannel, amqp_cstring_bytes(strQueueName.c_str()), iIfUnused, 0);
    if(0 != ErrorMsg(amqp_get_rpc_reply(m_pConn), "delete queue")) {
        return AMQP_ERROR;
    }

    return AMQP_OK;
}

int CRabbitmqClient::Publish(const string &strMessage, const string &strExchange, const string &strRoutekey) {
    if (NULL == m_pConn) {
        fprintf(stderr, "publish m_pConn is null, publish failed\n");
        return AMQP_ERROR;
    }

    amqp_bytes_t message_bytes;
    message_bytes.len = strMessage.length();
    message_bytes.bytes = (void *)(strMessage.c_str());
    //fprintf(stderr, "publish message(%d): %.*s\n", (int)message_bytes.len, (int)message_bytes.len, (char *)message_bytes.bytes);

    /*
    amqp_basic_properties_t props;
    props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
    props.content_type = amqp_cstring_bytes(m_type.c_str());
    props.delivery_mode = m_durable;    // persistent delivery mode
    */

    amqp_bytes_t exchange = amqp_cstring_bytes(strExchange.c_str());
    amqp_bytes_t routekey = amqp_cstring_bytes(strRoutekey.c_str());

    for(int i=0; i<m_iRetry; ++i) {
        //if (0 != amqp_basic_publish(m_pConn, m_iChannel, exchange, routekey, 0, 0, &props, message_bytes)) {
        if (0 == amqp_basic_publish(m_pConn, m_iChannel, exchange, routekey, 0, 0, NULL, message_bytes)) {
            return AMQP_OK;
        }
        fprintf(stderr, "publish amqp_basic_publish failed, retry\n");
    }

    // 处理失败
    fprintf(stderr, "publish amqp_basic_publish failed\n");
    ErrorMsg(amqp_get_rpc_reply(m_pConn), "amqp_basic_publish");
    return AMQP_ERROR;
}

int CRabbitmqClient::ConsumeNeedAck(const string &strQueueName, string &strMessage, uint64_t &ullAckTag, struct timeval *timeout) {
    if (NULL == m_pConn) {
        fprintf(stderr, "Consumer m_pConn is null, Consumer failed\n");
        return AMQP_ERROR;
    }

    if (!m_bBasicConsume) {
        int ack = 0; // no_ack    是否需要确认消息后再从队列中删除消息(0-需要确认 1-不需要确认)
        amqp_bytes_t queuename = amqp_cstring_bytes(strQueueName.c_str());
        amqp_basic_consume(m_pConn, m_iChannel, queuename, amqp_empty_bytes, 0, ack, 0, amqp_empty_table);

        if (0 != ErrorMsg(amqp_get_rpc_reply(m_pConn), "Consuming")) {
            fprintf(stderr, "Consumer amqp basic consume failed\n");
            return AMQP_ERROR;
        }

        m_bBasicConsume = true;
    }

    amqp_envelope_t envelope;
    amqp_maybe_release_buffers(m_pConn);
    amqp_rpc_reply_t res = amqp_consume_message(m_pConn, &envelope, timeout, 0);
    if (AMQP_RESPONSE_NORMAL != res.reply_type) {
        if (timeout != nullptr && res.library_error == AMQP_STATUS_TIMEOUT) {
            return AMQP_TIMEOUT;
        }
        fprintf(stderr, "Consumer amqp consume message failed\n");
        return -res.reply_type;
    }

    ullAckTag = envelope.delivery_tag;
    strMessage.assign((char *)envelope.message.body.bytes, (char *)envelope.message.body.bytes + envelope.message.body.len);

    amqp_destroy_envelope(&envelope);
    return AMQP_OK;
}

int CRabbitmqClient::ConsumeAck(uint64_t ullAckTag, int iMultiple) {
    if (NULL == m_pConn) {
        fprintf(stderr, "Consumer m_pConn is null, Consumer failed\n");
        return AMQP_ERROR;
    }
    
    int rtn = amqp_basic_ack(m_pConn, m_iChannel, ullAckTag, iMultiple);
    if (rtn != 0) {
        fprintf(stderr, "ConsumeAck amqp_basic_ack failed\n");
        return AMQP_ERROR;
    }

    return AMQP_OK;
}

int CRabbitmqClient::Consume(const string &strQueueName, vector<string> &vecMessage, int iGetNum, struct timeval *timeout) {
    if (NULL == m_pConn) {
        fprintf(stderr, "Consumer m_pConn is null, Consumer failed\n");
        return AMQP_ERROR;
    }

    if (!m_bBasicConsume) {
        amqp_basic_qos(m_pConn, m_iChannel, 0, iGetNum, 0);

        int ack = 0; // no_ack    是否需要确认消息后再从队列中删除消息(0-需要确认 1-不需要确认)
        amqp_bytes_t queuename = amqp_cstring_bytes(strQueueName.c_str());
        amqp_basic_consume(m_pConn, m_iChannel, queuename, amqp_empty_bytes, 0, ack, 0, amqp_empty_table);

        if (0 != ErrorMsg(amqp_get_rpc_reply(m_pConn), "Consuming")) {
            return AMQP_ERROR;
        }

        m_bBasicConsume = true;
    }


    int iHasget = 0;
    amqp_rpc_reply_t res;
    amqp_envelope_t envelope;
    while (iGetNum > 0) {
        amqp_maybe_release_buffers(m_pConn);
        res = amqp_consume_message(m_pConn, &envelope, timeout, 0);
        if (AMQP_RESPONSE_NORMAL != res.reply_type) {
            fprintf(stderr, "Consumer amqp consume message failed\n");

            if (0 == iHasget)
                return -res.reply_type;
            else
                return AMQP_OK;
        }

        string str((char *)envelope.message.body.bytes, (char *)envelope.message.body.bytes + envelope.message.body.len);
        vecMessage.push_back(str);
        int rtn = amqp_basic_ack(m_pConn, m_iChannel, envelope.delivery_tag, 1);
        amqp_destroy_envelope(&envelope);
        if (rtn != 0) {
            return AMQP_ERROR;
        }

        iGetNum--;
        iHasget++;
        usleep(1);
    }

    return AMQP_OK;
}

int CRabbitmqClient::ErrorMsg(amqp_rpc_reply_t x, char const *context) {
    switch (x.reply_type) {
        case AMQP_RESPONSE_NORMAL:
            return AMQP_OK;

        case AMQP_RESPONSE_NONE:
            fprintf(stderr, "%s: missing RPC reply type!\n", context);
            break;

        case AMQP_RESPONSE_LIBRARY_EXCEPTION:
            fprintf(stderr, "%s: %s\n", context, amqp_error_string2(x.library_error));
            break;

        case AMQP_RESPONSE_SERVER_EXCEPTION:
            switch (x.reply.id) {
                case AMQP_CONNECTION_CLOSE_METHOD: {
                    amqp_connection_close_t *m = (amqp_connection_close_t *)x.reply.decoded;
                    fprintf(stderr, "%s: server connection error %uh, message: %.*s\n",
                        context, m->reply_code, (int)m->reply_text.len,
                        (char *)m->reply_text.bytes);
                    break;
                                                    }
                case AMQP_CHANNEL_CLOSE_METHOD: {
                    amqp_channel_close_t *m = (amqp_channel_close_t *)x.reply.decoded;
                    fprintf(stderr, "%s: server channel error %uh, message: %.*s\n",
                        context, m->reply_code, (int)m->reply_text.len,
                        (char *)m->reply_text.bytes);
                    break;
                                                }
                default:
                    fprintf(stderr, "%s: unknown server error, method id 0x%08X\n",
                        context, x.reply.id);
                    break;
            }
            break;
    }

    return AMQP_ERROR;
}

