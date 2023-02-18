/**
 * @file ua_Server::cpp
 * @author zhaoxi (535394140@qq.com)
 * @brief
 * @version 1.0
 * @date 2023-01-28
 *
 * @copyright Copyright SCUT RobotLab(c) 2023
 *
 */

#include <iostream>
#include <thread>

#ifndef UA_ENABLE_AMALGAMATION
#include <open62541/plugin/log_stdout.h>
#endif //! UA_ENABLE_AMALGAMATION

#include <opencv2/core.hpp>

#include "asmpro/opcua_cs.hpp"

using namespace std;
using namespace cv;
using namespace ua;

// VisionTrigger
UA_StatusCode visionTrigger(UA_Server *server, const UA_NodeId *sessionId, void *sessionContext, const UA_NodeId *methodId,
                            void *methodContext, const UA_NodeId *objectId, void *objectContext, size_t inputSize,
                            const UA_Variant *input, size_t outputSize, UA_Variant *output)
{
    return UA_STATUSCODE_GOOD;
}

// ReloadConfig
UA_StatusCode reloadConfig(UA_Server *server, const UA_NodeId *sessionId, void *sessionContext, const UA_NodeId *methodId,
                           void *methodContext, const UA_NodeId *objectId, void *objectContext, size_t inputSize,
                           const UA_Variant *input, size_t outputSize, UA_Variant *output)
{
    return UA_STATUSCODE_GOOD;
}

// UpdateStatus
UA_StatusCode updateStatus(UA_Server *server, const UA_NodeId *sessionId, void *sessionContext, const UA_NodeId *methodId,
                           void *methodContext, const UA_NodeId *objectId, void *objectContext, size_t inputSize,
                           const UA_Variant *input, size_t outputSize, UA_Variant *output)
{
    return UA_STATUSCODE_GOOD;
}

bool is_running = true;

inline void onStop(int sig) { is_running = false; }

void changeImg()
{
    RNG rng(getTickCount());
    while (is_running)
    {
        this_thread::sleep_for(chrono::milliseconds(500));
        Mat img(Size(640, 480), CV_8UC3, Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)));
        Variable variable = convertImg(img);
        UA_NodeId node_id = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
        node_id = Server::findNodeId(node_id, 1, "VisionServer");
        node_id = Server::findNodeId(node_id, 1, "DeviceStatus[1]");
        node_id = Server::findNodeId(node_id, 1, "GrabImage");
        Server::writeVariable(node_id, variable);
    }

    exit(0);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, onStop);

    Server::init();
    // ########################### Image  Type ###########################
    // VariableType
    Variable image_var = convertImg(Mat(Size(640, 480), CV_8UC3, Scalar(0, 0, 0)));
    VariableType image_type(image_var);
    Server::addVariableTypeNode("ImageType", "Type of the image consisting of BGR888 and Mono8", image_type);

    // ########################## Vision Server ##########################
    // ObjectType
    ObjectType vision_server_type;
    UA_NodeId vision_server_type_id = Server::addObjectTypeNode("VisionServerType", "Type of the vision server",
                                                                vision_server_type);
    // Method
    vector<Argument> server_inputs = {Argument("Index", "The index of the specific device", UA_TYPES_UINT16)};
    Server::addMethodNode("VisionTrigger", "Trigger the specific device to process the vision program",
                          visionTrigger, server_inputs, null_args, vision_server_type_id);
    Server::addMethodNode("ReloadConfig", "Reload the specific device on the production line",
                          reloadConfig, server_inputs, null_args, vision_server_type_id);
    // Object
    Object vision_server(vision_server_type);
    UA_NodeId vision_server_id = Server::addObjectNode("VisionServer", "Vision server", vision_server,
                                                       vision_server_type_id);

    // ########################## Device Status ##########################
    // ObjectType
    ObjectType device_status_type;
    device_status_type.add("DeviceNumber", UA_UInt16(0));
    device_status_type.add("StatusMessage", "Normal Status");
    device_status_type.add("GrabImage", image_var);
    UA_NodeId device_status_type_id = Server::addObjectTypeNode("DeviceStatusType", "Type of the devices status",
                                                                device_status_type);
    // Method
    vector<UA_UInt32> dimension = {480, 640, 3};
    vector<Argument> status_inputs = {Argument("DeviceNumber", "The S/N or other number of the device", UA_TYPES_UINT16),
                                      Argument("StatusMessgae", "The status message of the device", UA_TYPES_STRING),
                                      Argument("GrabImage", "Image grabbed from the device", UA_TYPES_BYTE, dimension)};
    Server::addMethodNode("UpdateStatus", "Update the status of the device", updateStatus,
                          status_inputs, null_args, device_status_type_id);
    // Object
    vector<UA_NodeId> device_status(6);
    for (size_t i = 0; i < device_status.size(); ++i)
    {
        Object tmp(device_status_type);
        string i_ch = to_string(i);
        device_status[i] =
            Server::addObjectNode("DeviceStatus[" + i_ch + "]",
                                  "Status of the cameras and light controllers (No." + i_ch + ")",
                                  tmp, device_status_type_id, vision_server_id);
    }

    thread t1(changeImg);
    t1.detach();
    // ############################### Run ###############################
    Server::run();
}