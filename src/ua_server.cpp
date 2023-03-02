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

UA_StatusCode setExposure(UA_Server *server, const UA_NodeId *sessionId, void *sessionContext, const UA_NodeId *methodId,
                          void *methodContext, const UA_NodeId *objectId, void *objectContext, size_t inputSize,
                          const UA_Variant *input, size_t outputSize, UA_Variant *output)
{
    if (inputSize != 1)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Error inputSize in method: SetExposure.");
        return UA_STATUSCODE_BAD;
    }
    UA_NodeId exposure_id = Server::findNodeId(*objectId, 1, "Exposure");
    Variable val = Variable(input->data, input->type, input->arrayLength);
    Server::writeVariable(exposure_id, val);
    return UA_STATUSCODE_GOOD;
}

UA_StatusCode setGain(UA_Server *server, const UA_NodeId *sessionId, void *sessionContext, const UA_NodeId *methodId,
                      void *methodContext, const UA_NodeId *objectId, void *objectContext, size_t inputSize,
                      const UA_Variant *input, size_t outputSize, UA_Variant *output)
{
    if (inputSize != 1)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Error inputSize in method: SetGain.");
        return UA_STATUSCODE_BAD;
    }
    UA_NodeId gain_id = Server::findNodeId(*objectId, 1, "Gain");
    Variable val = Variable(input->data, input->type, input->arrayLength);
    Server::writeVariable(gain_id, val);
    return UA_STATUSCODE_GOOD;
}

UA_StatusCode setRedGain(UA_Server *server, const UA_NodeId *sessionId, void *sessionContext, const UA_NodeId *methodId,
                         void *methodContext, const UA_NodeId *objectId, void *objectContext, size_t inputSize,
                         const UA_Variant *input, size_t outputSize, UA_Variant *output)
{
    if (inputSize != 1)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Error inputSize in method: SetRedGain.");
        return UA_STATUSCODE_BAD;
    }
    UA_NodeId r_gain_id = Server::findNodeId(*objectId, 1, "RedGain");
    Variable val = Variable(input->data, input->type, input->arrayLength);
    Server::writeVariable(r_gain_id, val);
    return UA_STATUSCODE_GOOD;
}

UA_StatusCode setGreenGain(UA_Server *server, const UA_NodeId *sessionId, void *sessionContext, const UA_NodeId *methodId,
                           void *methodContext, const UA_NodeId *objectId, void *objectContext, size_t inputSize,
                           const UA_Variant *input, size_t outputSize, UA_Variant *output)
{
    if (inputSize != 1)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Error inputSize in method: SetGreenGain.");
        return UA_STATUSCODE_BAD;
    }
    UA_NodeId g_gain_id = Server::findNodeId(*objectId, 1, "GreenGain");
    Variable val = Variable(input->data, input->type, input->arrayLength);
    Server::writeVariable(g_gain_id, val);
    return UA_STATUSCODE_GOOD;
}

UA_StatusCode setBlueGain(UA_Server *server, const UA_NodeId *sessionId, void *sessionContext, const UA_NodeId *methodId,
                          void *methodContext, const UA_NodeId *objectId, void *objectContext, size_t inputSize,
                          const UA_Variant *input, size_t outputSize, UA_Variant *output)
{
    if (inputSize != 1)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Error inputSize in method: SetBlueGain.");
        return UA_STATUSCODE_BAD;
    }
    UA_NodeId b_gain_id = Server::findNodeId(*objectId, 1, "BlueGain");
    Variable val = Variable(input->data, input->type, input->arrayLength);
    Server::writeVariable(b_gain_id, val);
    return UA_STATUSCODE_GOOD;
}

UA_StatusCode setLuminance(UA_Server *server, const UA_NodeId *sessionId, void *sessionContext, const UA_NodeId *methodId,
                           void *methodContext, const UA_NodeId *objectId, void *objectContext, size_t inputSize,
                           const UA_Variant *input, size_t outputSize, UA_Variant *output)
{
    if (inputSize != 2)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Error inputSize in method: SetLuminance.");
        return UA_STATUSCODE_BAD;
    }
    UA_NodeId lum_id = Server::findNodeId(*objectId, 1, "Luminance");
    if (UA_NodeId_isNull(&lum_id))
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Null node id in method: SetLuminance.");
        return UA_STATUSCODE_BAD;
    }
    UA_UInt32 channel = *reinterpret_cast<UA_UInt32 *>(input[0].data);
    UA_Byte luminance = *reinterpret_cast<UA_Byte *>(input[1].data);
    UA_Variant val;
    UA_Server_readValue(server, lum_id, &val);
    UA_Byte *data = reinterpret_cast<UA_Byte *>(val.data);
    data[channel] = luminance;
    Server::writeVariable(lum_id, val);
    return UA_STATUSCODE_GOOD;
}

UA_StatusCode setDelay(UA_Server *server, const UA_NodeId *sessionId, void *sessionContext, const UA_NodeId *methodId,
                       void *methodContext, const UA_NodeId *objectId, void *objectContext, size_t inputSize,
                       const UA_Variant *input, size_t outputSize, UA_Variant *output)
{
    if (inputSize != 2)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Error inputSize in method: SetDelay.");
        return UA_STATUSCODE_BAD;
    }
    UA_NodeId del_id = Server::findNodeId(*objectId, 1, "Delay");
    if (UA_NodeId_isNull(&del_id))
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Null node id in method: SetDelay.");
        return UA_STATUSCODE_BAD;
    }
    UA_UInt32 channel = *reinterpret_cast<UA_UInt32 *>(input[0].data);
    UA_UInt16 delay = *reinterpret_cast<UA_UInt16 *>(input[1].data);
    UA_Variant val;
    UA_Server_readValue(server, del_id, &val);
    UA_UInt16 *data = reinterpret_cast<UA_UInt16 *>(val.data);
    data[channel] = delay;
    UA_Server_writeValue(server, del_id, val);
    return UA_STATUSCODE_GOOD;
}

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
    double gain = 0;
    while (is_running)
    {
        this_thread::sleep_for(chrono::milliseconds(500));
        Mat img(Size(640, 480), CV_8UC3, Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)));
        Variable img_val(img.data, &UA_TYPES[UA_TYPES_BYTE], img.cols * img.rows * img.channels());
        gain = gain > 3 ? 0 : gain + 0.01;
        UA_NodeId objects_folder_id = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
        UA_NodeId vision_server_id = Server::findNodeId(objects_folder_id, 1, "VisionServer");
        UA_NodeId camera1_id = Server::findNodeId(vision_server_id, 1, "Camera[1]");
        UA_NodeId image_id = Server::findNodeId(camera1_id, 1, "Image");
        UA_NodeId gain_id = Server::findNodeId(camera1_id, 1, "Gain");
        Server::writeVariable(image_id, img_val);
        Server::writeVariable(gain_id, static_cast<double>(gain));
    }

    exit(0);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, onStop);

    Server::init();
    // Image VariableType
    Mat img_data(Size(640, 480), CV_8UC3, Scalar(0, 0, 0));
    VariableType image(img_data.data, &UA_TYPES[UA_TYPES_BYTE], img_data.cols * img_data.rows * img_data.channels());
    UA_NodeId image_id =
        Server::addVariableTypeNode("ImageType", "Type of the image consisting of BGR888 and Mono8", image);
    // VisionDevice ObjectType
    ObjectType vision_device;
    vision_device.add("IP", "0.0.0.0");
    vision_device.add("Message", "No Message");
    UA_NodeId vision_device_id = Server::addObjectTypeNode("VisionDeviceType", "Type of VisionDevice", vision_device);
    // Camera ObjectType
    ObjectType camera;
    camera.add("Exposure", UA_UInt16(1000));
    camera.add("Gain", 1.0);
    camera.add("RedGain", 1.0);
    camera.add("GreenGain", 1.0);
    camera.add("BlueGain", 1.0);
    camera.add("Image", Variable(image), image_id);
    UA_NodeId camera_id = Server::addObjectTypeNode("CameraType", "Type of Camera",
                                                    camera, vision_device_id);
    vector<Argument> _exposure, _gain, _r_gain, _g_gain, _b_gain;
    _exposure.emplace_back("_Exposure", "Exposure of the camera", &UA_TYPES[UA_TYPES_UINT16]);
    _gain.emplace_back("_Gain", "Gain of the camera", &UA_TYPES[UA_TYPES_DOUBLE]);
    _r_gain.emplace_back("_RedGain", "Red gain of the camera", &UA_TYPES[UA_TYPES_DOUBLE]);
    _g_gain.emplace_back("_GreenGain", "Green gain of the camera", &UA_TYPES[UA_TYPES_DOUBLE]);
    _b_gain.emplace_back("_BlueGain", "Blue gain of the camera", &UA_TYPES[UA_TYPES_DOUBLE]);
    Server::addMethodNode("SetExposure", "Set exposure", setExposure, _exposure, null_args, camera_id);
    Server::addMethodNode("SetGain", "Set gain", setGain, _gain, null_args, camera_id);
    Server::addMethodNode("SetRedGain", "Set red gain", setRedGain, _r_gain, null_args, camera_id);
    Server::addMethodNode("SetGreenGain", "Set green gain", setGreenGain, _g_gain, null_args, camera_id);
    Server::addMethodNode("SetBlueGain", "Set blue gain", setBlueGain, _b_gain, null_args, camera_id);
    // LightController ObjectType
    ObjectType light_controller;
    vector<UA_Byte> luminance = {0, 0, 0, 0};
    vector<UA_UInt16> delay = {1U, 1U, 1U, 1U};
    light_controller.add("Luminance", Variable(luminance.data(), &UA_TYPES[UA_TYPES_BYTE], luminance.size()));
    light_controller.add("Delay", Variable(delay.data(), &UA_TYPES[UA_TYPES_UINT16], delay.size()));
    UA_NodeId light_controller_id = Server::addObjectTypeNode("LightControllerType", "Type of LightController",
                                                              light_controller, vision_device_id);
    vector<Argument> lum_args, del_args;
    lum_args.emplace_back("_Channel", "Specific channel", &UA_TYPES[UA_TYPES_UINT32]);
    lum_args.emplace_back("_Luminance", "Luminance", &UA_TYPES[UA_TYPES_BYTE]);
    del_args.emplace_back("_Channel", "Specific channel", &UA_TYPES[UA_TYPES_UINT32]);
    del_args.emplace_back("_Delay", "Specific channel", &UA_TYPES[UA_TYPES_UINT16]);
    Server::addMethodNode("SetLuminance", "Set luminance", setLuminance, lum_args, null_args, light_controller_id);
    Server::addMethodNode("SetDelay", "Set delay time", setDelay, del_args, null_args, light_controller_id);
    // VisionServer Object
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
    // Camera Object
    vector<Object> cameras;
    cameras.reserve(4);
    for (size_t i = 0; i < 4; ++i)
        cameras.push_back(Object(camera));
    for (size_t i = 0; i < cameras.size(); ++i)
        Server::addObjectNode("Camera[" + to_string(i) + "]", "Camera[" + to_string(i) + "]",
                              cameras[i], camera_id, vision_server_id);
    // LightController Object
    vector<Object> light_controllers;
    light_controllers.reserve(2);
    for (size_t i = 0; i < 2; ++i)
        light_controllers.push_back(Object(light_controller));
    for (size_t i = 0; i < light_controllers.size(); ++i)
        Server::addObjectNode("LightController[" + to_string(i) + "]", "LightController[" + to_string(i) + "]",
                              light_controllers[i], light_controller_id, vision_server_id);
    thread t1(changeImg);
    t1.detach();
    Server::run();
}
