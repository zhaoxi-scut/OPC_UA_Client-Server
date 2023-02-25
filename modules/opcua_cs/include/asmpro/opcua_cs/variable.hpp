/**
 * @file variable.hpp
 * @author 赵曦 (535394140@qq.com)
 * @brief Variable and variable type based on OPC UA protocal
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

//! @addtogroup opcua_cs_variable
//! @{

/**
 * @brief 基于 OPC UA 协议的变量类型
 * @note 通过 get() 方法可直接从 VariableType 获取其默认值
 */
class VariableType
{
    UA_Variant __init_val; //!< 默认值
    UA_UInt32 __size;      //!< 数组大小

public:
    VariableType() { UA_Variant_init(&__init_val); }
    ~VariableType() = default;

    template <typename _Tp>
    VariableType(const _Tp &val) : __size(0)
    {
        UA_Variant_init(&__init_val);
        initVal(&val, &getUaType<_Tp>());
    }

    VariableType(const void *val, const UA_DataType *type, std::size_t size = 0) : __size(size)
    {
        UA_Variant_init(&__init_val);
        initVal(val, type);
    }

    VariableType(const char *str) : __size(0)
    {
        UA_Variant_init(&__init_val);
        UA_String ua_str = UA_STRING(const_cast<char *>(str));
        initVal(&ua_str, &UA_TYPES[UA_TYPES_STRING]);
    }

    //! clone
    VariableType clone() const
    {
        VariableType retval;
        UA_Variant_copy(&__init_val, &retval.__init_val);
        retval.__size = __size;
        return retval;
    }

    //!< 获取 VariableType 的默认 UA_Variant 值
    inline const UA_Variant &get() const { return __init_val; }
    //!< 判空
    inline bool empty() { return UA_Variant_isEmpty(&__init_val); }

private:
    /**
     * @brief 初始化 UA_Variant
     * @note 在此函数调用之前，必须设置好 __size 成员变量
     * 
     * @param data 原始数据
     * @param type 单数据类型
     */
    void initVal(const void *data, const UA_DataType *type)
    {
        if (__size == 0)
        {
            UA_Variant_setScalarCopy(&__init_val, data, type);
            __init_val.arrayDimensionsSize = 0;
            __init_val.arrayDimensions = nullptr;
        }
        else
        {
            UA_Variant_setArrayCopy(&__init_val, data, __size, type);
            __init_val.arrayDimensionsSize = 1;
            __init_val.arrayDimensions = &__size;
        }
    }
};

/**
 * @brief 基于 OPC UA 协议的变量
 * @note 可直接从 VariableType 构造一个 Variable，也可通过与 VariableType
 *       同样的构造方式构造 Variable，Variable 的构造方式均为 
 */
class Variable
{
    UA_Variant __val; //!< UA_Variant
    UA_UInt32 __size; //!< 数组大小

public:
    Variable() { UA_Variant_init(&__val); }

    ~Variable() = default;

    explicit Variable(const VariableType &val_type)
    {
        UA_Variant_init(&__val);
        const UA_Variant &val = val_type.get();
        __size = val.arrayLength;
        initVal(val.data, val.type);
    }

    template <typename _Tp, typename Enable = std::enable_if_t<!std::is_same_v<_Tp, VariableType>>>
    Variable(const _Tp &val) : __size(0)
    {
        UA_Variant_init(&__val);
        initVal(&val, &getUaType<_Tp>());
    }

    Variable(const void *val, const UA_DataType *type, std::size_t size = 0) : __size(size)
    {
        UA_Variant_init(&__val);
        initVal(val, type);
    }

    Variable(const char *str) : __size(0)
    {
        UA_Variant_init(&__val);
        UA_String ua_str = UA_STRING(const_cast<char *>(str));
        initVal(&ua_str, &UA_TYPES[UA_TYPES_STRING]);
    }

    //! clone
    Variable clone() const
    {
        Variable retval;
        UA_Variant_copy(&__val, &retval.__val);
        retval.__size = __size;
        return retval;
    }

    //! 获取 UA_Variant 值
    inline const UA_Variant &get() const { return __val; }
    //! 判空
    inline bool empty() { return UA_Variant_isEmpty(&__val); }

private:
    /**
     * @brief 初始化 UA_Variant
     * @note 在此函数调用之前，必须设置好 __size 成员变量
     * 
     * @param data 原始数据
     * @param type 单数据类型
     */
    void initVal(const void *data, const UA_DataType *type)
    {
        if (__size == 0)
        {
            UA_Variant_setScalarCopy(&__val, data, type);
            __val.arrayDimensionsSize = 0;
            __val.arrayDimensions = nullptr;
        }
        else
        {
            UA_Variant_setArrayCopy(&__val, data, __size, type);
            __val.arrayDimensionsSize = 1;
            __val.arrayDimensions = &__size;
        }
    }
};

//! @} opcua_cs_variable

} // namespace ua
