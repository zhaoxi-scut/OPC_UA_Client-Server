/**
 * @file ua_client.cpp
 * @author zhaoxi (535394140@qq.com)
 * @brief 
 * @version 1.0
 * @date 2023-02-25
 * 
 * @copyright Copyright (c) 2023, zhaoxi
 * 
 */

#include <iostream>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "asmpro/opcua_cs/client.hpp"

using namespace std;
using namespace cv;
using namespace ua;

void imageChange(UA_Client *client, UA_UInt32 subId, void *subContext, UA_UInt32 monId,
                 void *monContext, UA_DataValue *value)
{
    UA_Variant v = value->value;
    if (v.arrayLength != 640 * 480 * 3)
        throw v.arrayLength;
    Mat img(Size(640, 480), CV_8UC3, v.data);
    imshow("img", img);
    waitKey(1);
}

int main(int argc, char *argv[])
{
    Client client;
    client.connect("opc.tcp://localhost:4840");
    UA_NodeId node_id = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    node_id = client.findNodeId(node_id, 1, "VisionServer");
    node_id = client.findNodeId(node_id, 1, "Camera[1]");
    node_id = client.findNodeId(node_id, 1, "Image");

    UA_UInt32 sub_id = client.createSubscription();
    client.createVariableMonitor(sub_id, node_id, imageChange);

    namedWindow("img");
    while (true)
        client.runIterate(0);

    return 0;
}
