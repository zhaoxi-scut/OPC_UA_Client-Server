/**
 * @file argument.hpp
 * @author 赵曦 (535394140@qq.com)
 * @brief
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

//! @addtogroup opcua_cs_argument
//! @{

/**
 * @brief Method argument based on OPC UA protocol
 */
class Argument
{
    UA_Argument __argument;           //!< Argument
    std::vector<UA_UInt32> __dim_arr; //!< Dimension array of variant

public:
    Argument(const Argument &arg) : __dim_arr(arg.__dim_arr) { UA_Argument_copy(&arg.__argument, &__argument); }
    Argument(Argument &&arg) : __dim_arr(arg.__dim_arr) { UA_Argument_copy(&arg.__argument, &__argument); }
    ~Argument() = default;

    /**
     * @brief Construct a new Argument object
     *
     * @param name Name of the argument
     * @param description Description of the argument
     * @param ua_types UA_TYPES_xxx
     * @param dimension_array Dimension array of variant
     */
    Argument(std::string name, std::string description, int ua_types,
             const std::vector<UA_UInt32> &dimension_array = std::vector<UA_UInt32>());

    /**
     * @brief Get argument
     * 
     * @return UA_Argument 
     */
    UA_Argument getArgument() const { return __argument; }
    
    /**
     * @brief Get dimension
     * 
     * @return std::size_t 
     */
    inline std::size_t getDimension() const { return __dim_arr.size(); }
};

const std::vector<Argument> null_args; //!< Null argument

//! @} opcua_cs_argument

} // namespace ua
