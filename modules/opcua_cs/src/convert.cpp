/**
 * @file convert.cpp
 * @author zhaoxi (535394140@qq.com)
 * @brief
 * @version 1.0
 * @date 2023-02-16
 *
 * @copyright Copyright SCUT RobotLab(c) 2023
 *
 */

#include "asmpro/opcua_cs/convert.h"

using namespace std;
using namespace cv;

namespace ua
{

Mat convertImg(const Variable &variable)
{
    UA_Variant variant = variable.getVariant();
    assert(variant.arrayDimensionsSize == 3);
    int rows = static_cast<int>(variant.arrayDimensions[0]);
    int cols = static_cast<int>(variant.arrayDimensions[1]);
    UA_UInt32 chns = variant.arrayDimensions[2];
    int type = chns == 1U ? CV_8UC1 : CV_8UC3;
    Mat retval(Size(cols, rows), type, variant.data);
    return retval;
}

Variable convertImg(const Mat &mat)
{
    Mat tmp = mat.clone();
    UA_UInt32 rows = static_cast<UA_UInt32>(tmp.rows);
    UA_UInt32 cols = static_cast<UA_UInt32>(tmp.cols);
    UA_UInt32 channels = static_cast<UA_UInt32>(tmp.channels());
    void *data = tmp.data;
    vector<UA_UInt32> dimensions = {rows, cols, channels};
    Variable retval(data, UA_TYPES_BYTE, READ | WRITE, dimensions);
    return retval;
}

} //! namespace ua
