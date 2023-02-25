/**
 * @file delaycal_server.cpp
 * @author zhaoxi (535394140@qq.com)
 * @brief
 * @version 1.0
 * @date 2023-02-25
 *
 * @copyright Copyright (c) 2023, zhaoxi
 *
 */

#include "asmpro/opcua_cs/server.hpp"

using namespace std;
using namespace ua;

UA_StatusCode timeTick(UA_Server *server, const UA_NodeId *sessionId,
                       void *sessionContext, const UA_NodeId *methodId,
                       void *methodContext, const UA_NodeId *objectId,
                       void *objectContext, size_t inputSize,
                       const UA_Variant *input, size_t outputSize,
                       UA_Variant *output)
{
    UA_DateTime tick = *reinterpret_cast<UA_DateTime *>(input[0].data);
    UA_Variant_setScalarCopy(output, &tick, &UA_TYPES[UA_TYPES_DATETIME]);
    return UA_STATUSCODE_GOOD;
}

int main(int argc, char *argv[])
{
    Server::init(4850);
    vector<Argument> inputs, outputs;
    inputs.emplace_back("InputByte", "time tick", &UA_TYPES[UA_TYPES_BYTE], 1280 * 960 * 3);
    outputs.emplace_back("OutputByte", "time tick", &UA_TYPES[UA_TYPES_BYTE], 1280 * 960 * 3);
    Server::addMethodNode("DelayCalculate", "Calculate the delay time",
                          timeTick, inputs, outputs);
    Server::run();
    return 0;
}
