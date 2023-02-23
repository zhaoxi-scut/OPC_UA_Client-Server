/**
 * @file ua_utility.hpp
 * @author 赵曦 (535394140@qq.com)
 * @brief
 * @version 1.0
 * @date 2022-09-16
 *
 * @copyright Copyright (c) 2023, zhaoxi
 *
 */

#pragma once

#include <string>

#include <open62541.h>

//! @addtogroup opcua_cs
//! @{

namespace ua
{

inline char *en_US = const_cast<char *>("en-US");
inline char *zh_CN = const_cast<char *>("zh-CN");

/**
 * @brief Change "std::string" to "char *"
 * 
 * @param str String type of "std::string"
 * @return String type of "char *"
 */
inline char *to_c(const std::string &str) { return const_cast<char *>(str.c_str()); }

//! Type to UA_DataType
template <typename _Tp>
const UA_DataType &getUaType();
//! UA_Boolean to UA_DataType
template <>
inline const UA_DataType &getUaType<UA_Boolean>() { return UA_TYPES[UA_TYPES_BOOLEAN]; }
//! UA_SByte to UA_DataType
template <>
inline const UA_DataType &getUaType<UA_SByte>() { return UA_TYPES[UA_TYPES_SBYTE]; }
//! UA_Byte to UA_DataType
template <>
inline const UA_DataType &getUaType<UA_Byte>() { return UA_TYPES[UA_TYPES_BYTE]; }
//! UA_Int16 to UA_DataType
template <>
inline const UA_DataType &getUaType<UA_Int16>() { return UA_TYPES[UA_TYPES_INT16]; }
//! UA_UInt16 to UA_DataType
template <>
inline const UA_DataType &getUaType<UA_UInt16>() { return UA_TYPES[UA_TYPES_UINT16]; }
//! UA_Int32 to UA_DataType
template <>
inline const UA_DataType &getUaType<UA_Int32>() { return UA_TYPES[UA_TYPES_INT32]; }
//! UA_UInt32 to UA_DataType
template <>
inline const UA_DataType &getUaType<UA_UInt32>() { return UA_TYPES[UA_TYPES_UINT32]; }
//! UA_UInt64 to UA_DataType
template <>
inline const UA_DataType &getUaType<UA_UInt64>() { return UA_TYPES[UA_TYPES_UINT64]; }
//! UA_Float to UA_DataType
template <>
inline const UA_DataType &getUaType<UA_Float>() { return UA_TYPES[UA_TYPES_FLOAT]; }
//! UA_Double to UA_DataType
template <>
inline const UA_DataType &getUaType<UA_Double>() { return UA_TYPES[UA_TYPES_DOUBLE]; }
//! UA_String to UA_DataType
template <>
inline const UA_DataType &getUaType<UA_String>() { return UA_TYPES[UA_TYPES_STRING]; }
//! UA_DateTime to UA_DataType
template <>
inline const UA_DataType &getUaType<UA_DateTime>() { return UA_TYPES[UA_TYPES_DATETIME]; }
//! UA_Guid to UA_DataType
template <>
inline const UA_DataType &getUaType<UA_Guid>() { return UA_TYPES[UA_TYPES_GUID]; }
//! UA_Variant to UA_DataType
template <>
inline const UA_DataType &getUaType<UA_Variant>() { return UA_TYPES[UA_TYPES_VARIANT]; }
//! UA_LocalizedText to UA_DataType
template <>
inline const UA_DataType &getUaType<UA_LocalizedText>() { return UA_TYPES[UA_TYPES_LOCALIZEDTEXT]; }

} // namespace ua

//! @} opcua_cs
