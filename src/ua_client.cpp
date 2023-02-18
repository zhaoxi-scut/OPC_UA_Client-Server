/**
 * @file ua_client.cpp
 * @author zhaoxi (535394140@qq.com)
 * @brief
 * @version 1.0
 * @date 2023-01-28
 *
 * @copyright Copyright SCUT RobotLab(c) 2023
 *
 */

#include <iostream>

#ifndef UA_ENABLE_AMALGAMATION
#include <open62541/plugin/log_stdout.h>
#endif //! UA_ENABLE_AMALGAMATION

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "asmpro/opcua_cs.hpp"

using namespace std;
using namespace cv;
using namespace ua;

void imageChange(UA_Client *client, UA_UInt32 subId, void *subContext, UA_UInt32 monId,
                 void *monContext, UA_DataValue *value)
{
    vector<UA_UInt32> dimensions(value->value.arrayDimensionsSize);
    for (size_t i = 0; i < value->value.arrayDimensionsSize; ++i)
        dimensions[i] = value->value.arrayDimensions[i];
    Variable img_data(value->value.data, UA_TYPES_BYTE, READ | WRITE, dimensions);
    Mat img = convertImg(img_data);
    imshow("img", img);
    waitKey(1);
}

int main(int argc, char *argv[])
{
    Client client;
    client.connect("opc.tcp://localhost:4840");
    UA_NodeId node_id = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    node_id = client.findNodeId(node_id, 1, "VisionServer");
    node_id = client.findNodeId(node_id, 1, "DeviceStatus[1]");
    node_id = client.findNodeId(node_id, 1, "GrabImage");

    UA_UInt32 sub_id = client.createSubscription();
    client.createVariableMonitor(sub_id, node_id, imageChange);

    namedWindow("img");
    while (true)
        client.runIterate(0);

    return 0;
}