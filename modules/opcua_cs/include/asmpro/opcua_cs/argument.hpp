/**
 * @file argument.hpp
 * @author 赵曦 (535394140@qq.com)
 * @brief
 * @version 1.0
 * @date 2022-09-17
 *
 * @copyright Copyright (c) 2023, zhaoxi
 *
 */

#pragma once

#include <vector>
#include <string>

#include "ua_utility.hpp"

namespace ua
{

//! @addtogroup opcua_cs_argument
//! @{

/**
 * @brief 基于 OPC UA 协议的方法参数
 */
class Argument
{
    UA_Argument __argument; //!< UA_Argument
    UA_UInt32 __size;       //!< 数组大小

public:
    Argument() = default;
    ~Argument() = default;

    /**
     * @brief 构造参数
     *
     * @param name 参数名
     * @param description 参数描述
     * @param type UA_DataType
     * @param size 数组大小 (default: 0)
     */
    Argument(const std::string &name, const std::string &description, const UA_DataType *type, UA_UInt32 size = 0);

    //! 获取 UA_Argument
    const UA_Argument &get() const { return __argument; }
};

const std::vector<Argument> null_args; //!< Null argument

//! @} opcua_cs_argument

} // namespace ua
