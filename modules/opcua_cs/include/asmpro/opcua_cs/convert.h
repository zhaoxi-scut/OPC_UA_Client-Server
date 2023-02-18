/**
 * @file convert.h
 * @author zhaoxi (535394140@qq.com)
 * @brief 
 * @version 1.0
 * @date 2023-02-16
 * 
 * @copyright Copyright SCUT RobotLab(c) 2023
 * 
 */

#pragma once

#include <opencv2/core/mat.hpp>

#include "variable.hpp"

//! @addtogroup opcua_cs
//! @{

namespace ua
{

/**
 * @brief Convert the type of the image 'ua::Variable' to 'cv::Mat'
 * 
 * @param variable Image type of ua::Variable
 * @return Image type of cv::Mat
 */
cv::Mat convertImg(const Variable &variable);

/**
 * @brief Convert the type of the image 'cv::Mat' to 'ua::Variable'
 * 
 * @param mat Image type of cv::Mat
 * @return Image type of ua::Variable
 */
Variable convertImg(const cv::Mat &mat);

} // namespace ua

//! @} opcua_cs
