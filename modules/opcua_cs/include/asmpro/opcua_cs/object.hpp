/**
 * @file argument.hpp
 * @author 赵曦 (535394140@qq.com)
 * @brief Object and object type based on OPC UA protocal
 * @version 1.0
 * @date 2022-09-17
 *
 * @copyright Copyright (c) 2023, zhaoxi
 *
 */

#pragma once

#include <unordered_map>

#include "variable.hpp"

namespace ua
{

//! @addtogroup opcua_cs_object
//! @{

/**
 * @brief 基于 OPC UA 协议的对象类型
 * @note 在执行 Server::addObjectTypeNode 时，会自动将 ObjectType 内部的数据变量添加至 Server
 *       中，并且数据都将添加一个 Mandatory 的属性，内部的 Method 添加必须使用 Server::addMethodNode
 *       方法，并指定 ObjectType 的 UA_NodeId，为 ObjectType 添加默认值可使用 ObjectType::add 方法
 */
class ObjectType
{
    std::unordered_map<std::string, Variable> __init_name_var; //!< 哈希表 [浏览信息名: 变量数据]

public:
    ObjectType() = default;
    ~ObjectType() = default;

    /**
     * @brief 为变量类型添加初始变量
     *
     * @param browse_name 变量的浏览信息名
     * @param default_var 需要添加至对象的默认变量数据
     */
    inline void add(const std::string &browse_name, const Variable &default_var)
    {
        __init_name_var[browse_name] = default_var;
    }

    //! 获取哈希表 [浏览信息名: 变量数据]
    inline const auto &get() const { return __init_name_var; }
};

/**
 * @brief 基于 OPC UA 协议的对象
 * @note Object 的生成完全依赖于 ObjectType，直接构造一个 Object 并且不做任何修改，Server::addObjectNode
 *       方法可将 ObjectType 中设置为 Mandatory 属性的变量、方法在 Object 中创建一份，Object
 *       中默认值的修改可使用 Object::add 方法
 */
class Object
{
    std::unordered_map<std::string, Variable> __name_var;

public:
    /**
     * @brief 创建新的对象
     *
     * @param obj_type 对象类型数据
     */
    explicit Object(const ObjectType &obj_type) : __name_var(obj_type.get()) {}

    ~Object() = default;

    /**
     * @brief 为对象添加变量
     * @note 如果该对象所代表的对象类型中不存在要添加的变量名，则添加失败
     *
     * @param browse_name 变量的浏览信息名
     * @param variable 需要添加至对象的变量数据
     */
    inline void add(const std::string &browse_name, const Variable &variable)
    {
        if (__name_var.find(browse_name) != __name_var.end())
            __name_var[browse_name] = variable;
    }

    //!< 获取浏览信息名及对应的变量数据
    inline const auto &getNameVar() const { return __name_var; }
};

/**
 * @brief 基于 OPC UA 协议的事件类型
 * @note 事件节点同样为 ObjectTypeNode，但其 parentNode 一定是 BaseEventType，
 *       并且其包含的数据的 DataType 必须是 Property 类型而非 Attribute 类型
 */
class EventType : public ObjectType
{
};

//! @} opcua_cs_object

} // namespace ua
