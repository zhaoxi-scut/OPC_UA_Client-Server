/**
 * @file client.hpp
 * @author 赵曦 (535394140@qq.com)
 * @brief Industrial Internet client based on OPC UA protocol
 * @version 1.0
 * @date 2022-09-22
 *
 * @copyright Copyright (c) 2023, zhaoxi
 *
 */

#pragma once

#include "argument.hpp"
#include "object.hpp"
#include "variable.hpp"

namespace ua
{

//! @addtogroup opcua_cs
//! @{

/**
 * @brief 基于 OPC UA 协议的工业互联网客户端
 */
class Client final
{
    UA_Client *__client;             //!< 客户端指针
    UA_Boolean __is_connect = false; //!< 是否已连接

public:
    //! 创建新的客户端对象
    Client();
    
    //! 销毁客户端对象
    ~Client()
    {
        disconnect();
        UA_Client_delete(__client);
    }

    /**
     * @brief 断开服务器 - 客户端连接
     */
    inline void disconnect() { UA_Client_disconnect(__client), __is_connect = UA_FALSE; }

    /**
     * @brief 监听网络并在后台处理到达的异步响应。还完成了内部管理、SecureChannels 的更新和订阅管理。
     *
     * @param timeOut 服务器响应超时时间 (单位：ms)
     */
    inline void runIterate(UA_UInt32 timeOut) { UA_Client_run_iterate(__client, timeOut); }

    /**
     * @brief 将客户端连接至指定的服务器
     *
     * @param address 服务器地址 (opc.tcp://xxxx:port)
     * @param username 用户名 (default: "")
     * @param password 密码 (default: "")
     * @return 是否成功完成当前操作的状态码
     */
    UA_Boolean connect(const std::string &address, const std::string &username = "", const std::string &password = "");

    /**
     * @brief 客户端路径搜索，获取目标节点 ID
     * @note 变量、对象、方法默认起始节点 ID: ns=0, s=UA_NS0ID_OBJECTSFOLDER
     * @note 变量类型默认起始节点 ID: ns=0, s=UA_NS0ID_BASEDATAVARIABLETYPE
     * @note 对象类型默认起始节点 ID: ns=0, s=UA_NS0ID_BASEOBJECTTYPE
     *
     * @param origin_id 起始节点 ID
     * @param target_ns 目标节点命名空间编号
     * @param browse_name 目标节点名 (Qualified Name)
     * @return 目标节点 ID
     */
    UA_NodeId findNodeId(const UA_NodeId &origin_id, const UA_UInt32 target_ns, const std::string &target_name);

    /**
     * @brief 把值写入服务器中的变量节点
     * 
     * @param node_id 变量节点 ID
     * @param data 变量数据信息
     * @return 是否成功完成当前操作的状态码 
     */
    UA_Boolean writeVariable(const UA_NodeId &node_id, const Variable &data);

    /**
     * @brief 从服务器中读取指定的变量节点
     * @note 返回值的变量类型为可读可写权限
     *
     * @param node_id 变量节点 ID
     * @return 指定的变量
     */
    Variable readVariable(const UA_NodeId &node_id);

    /**
     * @brief 在客户端调用服务器中的指定方法
     *
     * @param node_id 方法节点 ID
     * @param inputs 输入参数列表
     * @param outputs 输出参数列表
     * @param parent_id 父对象节点 ID (default: ns=0, s=UA_NS0ID_OBJECTSFOLDER)
     * @return 是否成功完成当前操作的状态码
     */
    UA_Boolean call(const UA_NodeId &node_id, const std::vector<Variable> &inputs, std::vector<Variable> &outputs,
                    const UA_NodeId &parent_id = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER));

    /**
     * @brief 创建订阅请求
     * 
     * @return 订阅编号
     */
    UA_UInt32 createSubscription();

    /**
     * @brief 创建变量节点监视项
     * @note 当所监测的服务器中的变量数据发生更改时，通知监视器，执行 data_change 回调函数
     *
     * @param sub_id 订阅请求 ID
     * @param node_id 待监视的节点 ID（一般是变量节点 ID，成员变量需要使用 findChildId 进行路径搜索）
     * @param data_change_handler 数据变更回调函数
     * @return 是否成功完成当前操作的状态码
     */
    UA_Boolean createVariableMonitor(UA_UInt32 sub_id, UA_NodeId node_id,
                                     UA_Client_DataChangeNotificationCallback data_change_handler);

    /**
     * @brief 创建事件属性监视项
     * 
     * @param sub_id 订阅请求 ID
     * @param node_id 待监视的节点 ID (Server ID: ns=0, s=UA_NS0ID_SERVER)
     * @param names QualifiledNames 列表
     * @param event_handler 事件回调函数
     * @return 是否成功完成当前操作的状态码 
     */
    UA_Boolean createEventMonitor(UA_UInt32 sub_id, UA_NodeId node_id, std::vector<std::string> &names,
                                  UA_Client_EventNotificationCallback event_handler);
};

//! @} opcua_cs

} // namespace ua
