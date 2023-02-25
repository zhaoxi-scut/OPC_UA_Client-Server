/**
 * @file server.hpp
 * @author 赵曦 (535394140@qq.com)
 * @brief Industrial Internet server based on OPC UA protocol
 * @version 1.0
 * @date 2022-09-16
 *
 * @copyright Copyright (c) 2023, zhaoxi
 *
 */

#pragma once

#include <unordered_set>

#include "argument.hpp"
#include "object.hpp"
#include "variable.hpp"

namespace ua
{

//! @addtogroup opcua_cs
//! @{

/**
 * @brief 基于 OPC UA 协议的工业互联网服务器
 */
class Server final
{
    static void stopHandler(int sign);

    //! 值回调函数，Read 函数指针定义
    using ValueCallBackRead = void (*)(UA_Server *, const UA_NodeId *, void *, const UA_NodeId *, void *,
                                       const UA_NumericRange *, const UA_DataValue *);
    //! 值回调函数，Write 函数指针定义
    using ValueCallBackWrite = void (*)(UA_Server *, const UA_NodeId *, void *, const UA_NodeId *, void *,
                                        const UA_NumericRange *, const UA_DataValue *);
    //! 数据源回调函数，Read 函数指针定义
    using DataSourceRead = UA_StatusCode (*)(UA_Server *, const UA_NodeId *, void *, const UA_NodeId *, void *,
                                             UA_Boolean, const UA_NumericRange *, UA_DataValue *);
    //! 数据源回调函数，Write 函数指针定义
    using DataSourceWrite = UA_StatusCode (*)(UA_Server *, const UA_NodeId *, void *, const UA_NodeId *, void *,
                                              const UA_NumericRange *, const UA_DataValue *);

    static UA_Server *__server;  //!< OPC UA 服务器指针
    static UA_Boolean __is_init; //!< 服务器初始化状态
    static UA_Boolean __running; //!< 服务器运行状态

#ifndef NDEBUG
#define SERVER_RUNNING_ASSERT()                                                         \
    do                                                                                  \
    {                                                                                   \
        if (__running)                                                                  \
        {                                                                               \
            printf("Assertion: \033[31mServer is running.\033[0m On line %u of \"%s\"", \
                   __LINE__, __FILE__);                                                 \
            abort();                                                                    \
        }                                                                               \
    } while (false)

#define SERVER_INIT_ASSERT()                                                                   \
    do                                                                                         \
    {                                                                                          \
        if (!__is_init)                                                                        \
        {                                                                                      \
            printf("Assertion: \033[31mServer isn't initialized.\033[0m On line %u of \"%s\"", \
                   __LINE__, __FILE__);                                                        \
            abort();                                                                           \
        }                                                                                      \
    } while (false)

#else
#define SERVER_RUNNING_ASSERT() void(0)
#define SERVER_INIT_ASSERT() void(0)

#endif //! NDEBUG

public:
    Server() = default;
    Server(const Server &) = delete;
    Server(Server &&) = delete;

    /**
     * @brief Destroy the Server object
     */
    ~Server() { stop(); }

    /**
     * @brief 初始化服务器
     * @note 用户名列表与密码列表数目应该一致
     *
     * @param port OPC UA 服务器端口号，默认值为 '4840'
     * @param user_name 用户名列表
     * @param password 密码列表
     */
    static void init(UA_UInt16 port = 4840U, const std::vector<std::string> &user_name = {},
                     const std::vector<std::string> &password = {});

    /**
     * @brief 运行服务器
     * @note 此函数需要在初始化服务器配置之后再运行
     */
    static void run();

    /**
     * @brief 手动终止服务器
     */
    static void stop() { UA_Server_delete(__server); }

    /**
     * @brief 服务端路径搜索，获取目标节点 ID
     * @param origin_id 起始节点 ID，
     *                  变量、对象、方法默认起始节点 ID: ns=0, s=UA_NS0ID_OBJECTSFOLDER，
     *                  变量类型默认起始节点 ID: ns=0, s=UA_NS0ID_BASEDATAVARIABLETYPE，
     *                  对象类型默认起始节点 ID: ns=0, s=UA_NS0ID_BASEOBJECTTYPE，
     *                  事件类型默认起始节点 ID: ns=0, s=UA_NS0ID_BASEEVENTTYPE
     * @param target_ns 目标节点命名空间编号
     * @param target_name 目标节点名 (Qualified Name)
     * @return 目标节点 ID
     */
    static UA_NodeId findNodeId(const UA_NodeId &origin_id, UA_UInt32 target_ns, const std::string &target_name);

    /**
     * @brief 添加变量节点至 OPC UA 服务器中
     *
     * @param browse_name 变量的浏览信息名
     * @param description 变量的描述
     * @param data 变量数据信息
     * @param type_id 变量类型节点 ID (default: ns=0, s=UA_NS0ID_BASEDATAVARIABLETYPE)
     * @return 添加的节点 ID
     */
    static UA_NodeId addVariableNode(const std::string &browse_name, const std::string &description, const Variable &data,
                                     const UA_NodeId &type_id = UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE));

    /**
     * @brief 把值写入服务器中的变量节点
     *
     * @param node_id 变量节点 ID
     * @param data 变量数据信息
     * @return 是否成功写入变量节点
     */
    static UA_Boolean writeVariable(const UA_NodeId &node_id, const Variable &data);

    /**
     * @brief 从服务器中读取指定的变量节点
     *
     * @param node_id 变量节点 ID
     * @return 指定的变量
     */
    static Variable readVariable(const UA_NodeId &node_id);

    /**
     * @brief 为变量节点添加值回调函数
     *
     * @param node_id 变量节点 ID
     * @param before_read 值回调，在读取之前执行
     * @param after_write 值回调，在写入之后
     */
    static void addVariableNodeValueCallBack(const UA_NodeId &node_id, ValueCallBackRead before_read,
                                             ValueCallBackWrite after_write);

    /**
     * @brief 添加数据源节点至 OPC UA 服务器中
     *
     * @param browse_name 变量的浏览信息名
     * @param description 变量的描述
     * @param data 变量数据信息
     * @param on_read 数据源回调，在读取时执行
     * @param on_write 数据源回调，在写入时执行
     * @param type_id 变量类型节点 ID (default: ns=0, s=UA_NS0ID_BASEDATAVARIABLETYPE)
     * @return 添加的节点 ID
     */
    static UA_NodeId addDataSourceVariableNode(const std::string &browse_name, const std::string &description,
                                               const Variable &data, DataSourceRead on_read, DataSourceWrite on_write,
                                               const UA_NodeId &type_id = UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE));

    /**
     * @brief 创建变量节点添加监测项
     * @note 此操作会执行对指定节点的订阅操作，并监视一个变量，每隔一个采样间隔会查看此变量。
     *       当所监视的变量值发生改变，会执行对应的数据更改回调函数
     *
     * @param node_id 变量节点 ID
     * @param data_change 数据更改回调函数
     * @param sampling_interval 采样间隔
     */
    static void createVariableMonitor(UA_NodeId &node_id, UA_Server_DataChangeNotificationCallback data_change,
                                      UA_Double sampling_interval);

    /**
     * @brief 添加变量类型节点至 OPC UA 服务器中
     *
     * @param browse_name 变量类型的浏览信息名
     * @param description 变量类型的描述
     * @param data 变量类型数据信息
     * @return 添加的节点 ID
     */
    static UA_NodeId addVariableTypeNode(const std::string &browse_name, const std::string &description,
                                         const VariableType &valType);

    /**
     * @brief 添加对象节点至 OPC UA 服务器中
     *
     * @param browse_name 对象的浏览信息名
     * @param description 对象的描述
     * @param data 对象数据信息
     * @param type_id 对象类型节点 (default: ns=0, s=UA_NS0ID_BASEOBJECTTYPE)
     * @param parent_id 父对象节点 ID (default: ns=0, s=UA_NS0ID_OBJECTSFOLDER)
     * @return 添加的节点 ID
     */
    static UA_NodeId addObjectNode(const std::string &browse_name,
                                   const std::string &description, const Object &data,
                                   const UA_NodeId &type_id = UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
                                   const UA_NodeId &parent_id = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER));

    /**
     * @brief 添加对象类型节点至 OPC UA 服务器中
     *
     * @param browse_name 对象类型的浏览信息名
     * @param description 对象类型的描述
     * @param data 对象类型数据信息
     * @param parent_id 父对象节点 ID (default: )
     * @return 添加的节点 ID
     */
    static UA_NodeId addObjectTypeNode(const std::string &browse_name,
                                       const std::string &description, const ObjectType &data,
                                       UA_NodeId parent_id = UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE));

    /**
     * @brief 添加事件类型节点至 OPC UA 服务器中
     * @note 自定义的事件类型继承于 BaseEventType，基类的 Mandatory
     *       数据在自定义事件类型内部已经包含，包括: SourceName, Time,
     *       ReceiveTime, Message, Severity 等内容，参数传入的 data
     *       为额外信息
     *
     * @param browse_name 事件类型的浏览信息名
     * @param description 事件类型的描述
     * @param data 事件类型数据信息，默认为空
     * @return 添加的节点 ID
     */
    static UA_NodeId addEventTypeNode(const std::string &browse_name, const std::string &description,
                                      const EventType &data = EventType());

    /**
     * @brief 添加方法节点至 OPC UA 服务器中
     *
     * @param browse_name 方法的浏览信息名
     * @param description 方法的描述
     * @param on_method 方法回调函数体
     * @param input_args 输入参数列表
     * @param output_args 输出参数列表
     * @param parent_id 父对象节点 ID (default: ns=0, s=UA_NS0ID_OBJECTSFOLDER)
     * @return 添加的节点 ID
     */
    static UA_NodeId addMethodNode(const std::string &browse_name, const std::string &description, UA_MethodCallback on_method,
                                   const std::vector<Argument> &input_args, const std::vector<Argument> &output_args,
                                   const UA_NodeId &parent_id = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER));

    /**
     * @brief 创建事件
     * 
     * @param event_type_id 事件类型节点 ID
     * @return 事件节点 ID
     */
    static UA_NodeId createEvent(const UA_NodeId &event_type_id);

    /**
     * @brief 写入对象、事件属性
     * 
     * @param node_id 对象、事件节点 ID
     * @param target_name 目标属性节点名 (Qualified Name)
     * @param data 属性变量数据信息
     * @return 是否成功写入对象、事件属性
     */
    static UA_Boolean writeProperty(const UA_NodeId &node_id, const std::string &target_name, const Variable &data);

    /**
     * @brief 服务器触发事件
     * @note 事件在触发后，节点 ID 将被自动释放
     * 
     * @param node_id 事件节点 ID
     * @param origin_id 事件发出者节点 ID (default: ns=0, s=UA_NS0ID_SERVER) 
     * @return 是否成功触发事件
     */
    static UA_Boolean triggerEvent(const UA_NodeId &node_id, const UA_NodeId &origin_id = UA_NODEID_NUMERIC(0, UA_NS0ID_SERVER));

private:
    /**
     * @brief 在添加变量节点之前配置变量属性
     *
     * @param browse_name 变量的浏览信息名
     * @param description 变量的描述
     * @param data 变量数据
     * @return 变量的 Attributes 属性
     */
    static UA_VariableAttributes configVariableAttribute(const std::string &name, const std::string &description,
                                                         const Variable &data);
};

inline UA_Server *Server::__server = nullptr;
inline UA_Boolean Server::__is_init = UA_FALSE;
inline UA_Boolean Server::__running = UA_FALSE;

//! @} opcua_cs

} // namespace ua
