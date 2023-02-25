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
#include <csignal>
#include <thread>

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
    vector<double> position = {1.1, 2.2};
    UA_Variant_setArrayCopy(&output[0], position.data(), position.size(), &UA_TYPES[UA_TYPES_DOUBLE]);
    double angle = 3.3;
    UA_Variant_setScalarCopy(&output[1], &angle, &UA_TYPES[UA_TYPES_DOUBLE]);
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
        Variable variable(img.data, &UA_TYPES[UA_TYPES_BYTE], img.cols * img.rows * img.channels());
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
    // ImageType
    Mat image(Size(640, 480), CV_8UC3, Scalar(0, 0, 0));
    VariableType image_type(image.data, &UA_TYPES[UA_TYPES_BYTE], image.cols * image.rows * image.channels());
    UA_NodeId image_type_id =
        Server::addVariableTypeNode("ImageType", "Type of the image consisting of BGR888 and Mono8", image_type);
    // VisionDeviceType
    ObjectType vision_device_type;
    vision_device_type.add("IP", "0.0.0.0");
    vision_device_type.add("Message", "No Message");
    UA_NodeId vision_device_type_id = Server::addObjectTypeNode("VisionDeviceType", "Type of VisionDevice", vision_device_type);
    // CameraType
    ObjectType camera_type;
    camera_type.add("Exposure", UA_UInt16(1000));
    camera_type.add("Gain", 1.0);
    camera_type.add("RedGain", 1.0);
    camera_type.add("GreenGain", 1.0);
    camera_type.add("BlueGain", 1.0);
    camera_type.add("Image", Variable(image_type), image_type_id);
    UA_NodeId camera_type_id = Server::addObjectTypeNode("CameraType", "Type of Camera",
                                                         camera_type, vision_device_type_id);
    // LightControllerType
    ObjectType light_controller_type;
    vector<UA_Byte> luminance = {0, 0, 0, 0};
    vector<UA_Byte> delay = {1, 1, 1, 1};
    light_controller_type.add("Luminance", Variable(luminance.data(), &UA_TYPES[UA_TYPES_BYTE], luminance.size()));
    light_controller_type.add("Delay", Variable(delay.data(), &UA_TYPES[UA_TYPES_BYTE], delay.size()));
    UA_NodeId light_controller_type_id = Server::addObjectTypeNode("LightControllerType", "Type of LightController",
                                                                   light_controller_type, vision_device_type_id);
    // VisionServer
    Object vision_server;
    UA_NodeId vision_server_id = Server::addObjectNode("VisionServer", "Vision server ", vision_server);
    vector<Argument> server_inputs;
    server_inputs.reserve(3);
    server_inputs.emplace_back("CameraIdx", "Index of the camera", &UA_TYPES[UA_TYPES_UINT16]);
    server_inputs.emplace_back("ControllerIdx", "Index of the light controller", &UA_TYPES[UA_TYPES_UINT16]);
    server_inputs.emplace_back("ChannelIdx", "Channel of the light controller", &UA_TYPES[UA_TYPES_BYTE]);
    vector<Argument> server_outputs;
    server_outputs.reserve(2);
    server_outputs.emplace_back("TargetPosition", "Position of the target", &UA_TYPES[UA_TYPES_DOUBLE], 2);
    server_outputs.emplace_back("TargetAngle", "Delta angle of the target", &UA_TYPES[UA_TYPES_DOUBLE]);
    Server::addMethodNode("VisionTrigger", "Trigger the specific device to process the vision program",
                          visionTrigger, server_inputs, server_outputs, vision_server_id);
    // Camera
    vector<Object> cameras;
    cameras.reserve(4);
    for (size_t i = 0; i < 4; ++i)
        cameras.push_back(Object(camera_type));
    for (size_t i = 0; i < cameras.size(); ++i)
        Server::addObjectNode("Camera[" + to_string(i) + "]", "Camera[" + to_string(i) + "]",
                              cameras[i], camera_type_id, vision_server_id);
    // LightController
    vector<Object> light_controllers;
    light_controllers.reserve(2);
    for (size_t i = 0; i < 2; ++i)
        light_controllers.push_back(Object(light_controller_type));
    for (size_t i = 0; i < light_controllers.size(); ++i)
        Server::addObjectNode("LightController[" + to_string(i) + "]", "LightController[" + to_string(i) + "]",
                              light_controllers[i], light_controller_type_id, vision_server_id);
    // thread t1(changeImg);
    // t1.detach();
    Server::run();
}
