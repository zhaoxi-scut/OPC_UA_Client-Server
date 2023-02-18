/**
 * @file variable.hpp
 * @author 赵曦 (535394140@qq.com)
 * @brief Variable and variable type based on OPC UA protocal
 * @version 1.0
 * @date 2022-09-17
 *
 * @copyright Copyright SCUT RobotLab(c) 2022
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
 * @brief Variable based on OPC UA protocal
 */
class Variable
{
    UA_Variant __variant;             //!< Variant
    std::vector<UA_UInt32> __dim_arr; //!< Dimension array of variant
    UA_Byte __access_level;           //!< Access level of this variable in server or client

public:
    Variable() { UA_Variant_init(&__variant); }

    ~Variable() = default;

    Variable(const Variable &v) : __dim_arr(v.__dim_arr), __access_level(v.__access_level)
    {
        UA_Variant_copy(&v.__variant, &__variant);
    }

    Variable(Variable &&v) : __dim_arr(v.__dim_arr), __access_level(v.__access_level)
    {
        UA_Variant_copy(&v.__variant, &__variant);
    }

    void operator=(const Variable &v)
    {
        __dim_arr = v.__dim_arr;
        __access_level = v.__access_level;
        UA_Variant_copy(&v.__variant, &__variant);
    }

    /**
     * @brief Construct a new Variable object
     *
     * @tparam _InterType built-in type
     * @param val Built-in type data
     * @param access_level Accessibility of node data
     */
    template <typename _InterType>
    Variable(const _InterType &val, UA_Byte access_level = READ | WRITE)
        : __access_level(access_level)
    {
        UA_Variant_init(&__variant);
        UA_Variant_setScalar(&__variant, const_cast<_InterType *>(&val), &getUaType<_InterType>());
    }

    /**
     * @brief Construct a new Variable object
     *
     * @param str string data
     * @param access_level Accessibility of node data
     */
    Variable(const char *str, UA_Byte access_level = READ | WRITE)
        : __access_level(access_level)
    {
        UA_Variant_init(&__variant);
        UA_String ua_str = UA_STRING(const_cast<char *>(str));
        UA_Variant_setScalarCopy(&__variant, &ua_str, &UA_TYPES[UA_TYPES_STRING]);
    }

    /**
     * @brief Generating variable from raw data
     *
     * @param data Raw data
     * @param ua_types UA_TYPES_xxx
     * @param access_level Accessibility of node data
     * @param dimension_array Dimension array of variant
     */
    Variable(void *data, UA_UInt32 ua_types, UA_Byte access_level,
             std::vector<UA_UInt32> &dimension_array);

    //!< Get pointer of variant
    inline const UA_Variant &getVariant() const { return __variant; }
    //!< Get dimension
    inline std::size_t getDimension() const { return __dim_arr.size(); }
    //!< Get access level
    inline UA_Byte getAccess() const { return __access_level; }
};

//!< Variable type based on OPC UA protocal
class VariableType
{
    UA_Variant __init_variant;        //!< The initial value
    std::vector<UA_UInt32> __dim_arr; //!< Dimension array of variant

public:
    /**
     * @brief Construct a new Variable Type object
     */
    VariableType()
    {
        UA_Variant_init(&__init_variant);
    }

    ~VariableType() = default;

    /**
     * @brief Construct a new Variable Type object
     *
     * @param vt Another instance of variable type
     */
    VariableType(const VariableType &vt) : __dim_arr(vt.__dim_arr)
    {
        UA_Variant_copy(&vt.__init_variant, &__init_variant);
    }

    /**
     * @brief Construct a new Variable Type object
     *
     * @param vt Another instance of variable type
     */
    VariableType(VariableType &&vt) : __dim_arr(vt.__dim_arr)
    {
        UA_Variant_copy(&vt.__init_variant, &__init_variant);
    }

    /**
     * @brief operator =
     *
     * @param vt Another instance of variable type
     */
    void operator=(const VariableType &vt)
    {
        __dim_arr = vt.__dim_arr;
        UA_Variant_copy(&vt.__init_variant, &__init_variant);
    }

    /**
     * @brief Construct a new Variable Type object
     *
     * @param nameId Name of variable type nodeId
     * @param variable Variable
     */
    explicit VariableType(Variable &variable);

    //!< Get pointer of default variant
    inline const UA_Variant &getDefault() const { return __init_variant; }
    //!< Get dimension
    inline std::size_t getDimension() const { return __dim_arr.size(); }
};

//! @} opcua_cs_variable

} // namespace ua
