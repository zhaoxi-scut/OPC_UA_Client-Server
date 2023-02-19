/**
 * @file variable.cpp
 * @author 赵曦 (535394140@qq.com)
 * @brief Variable and variable type based on OPC UA protocal
 * @version 1.0
 * @date 2022-09-17
 *
 * @copyright Copyright (c) 2023, zhaoxi
 *
 */

#include "asmpro/opcua_cs/variable.hpp"

using namespace std;
using namespace ua;

/**
 * @brief Generating variable from raw data
 *
 * @param data Raw data
 * @param ua_types UA_TYPES_xxx
 * @param access_level Accessibility of node data
 * @param dimension_array Dimension array of variant
 */
Variable::Variable(void *data, UA_UInt32 ua_types, UA_Byte access_level,
                   vector<UA_UInt32> &dimension_array)
    : __access_level(access_level)
{
    UA_Variant_init(&__variant);
    if (dimension_array.empty())
        UA_Variant_setScalar(&__variant, data, &UA_TYPES[ua_types]);
    else
    {
        size_t data_size = 1U;
        for (auto dim : dimension_array)
            data_size *= dim;
        UA_Variant_setArrayCopy(&__variant, data, data_size, &UA_TYPES[ua_types]);
    }
    __dim_arr = dimension_array;
    __variant.arrayDimensions = __dim_arr.data();
    __variant.arrayDimensionsSize = __dim_arr.size();
}

/**
 * @brief Construct a new Variable Type object
 *
 * @param variable Default variable
 */
VariableType::VariableType(Variable &variable)
{
    UA_Variant_copy(&variable.getVariant(), &__init_variant);
    __dim_arr.reserve(__init_variant.arrayDimensionsSize);
    for (std::size_t i = 0; i < __init_variant.arrayDimensionsSize; ++i)
        __dim_arr.emplace_back(__init_variant.arrayDimensions[i]);
}
