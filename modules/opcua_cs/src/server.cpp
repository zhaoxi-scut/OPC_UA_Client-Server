/**
 * @file main.cpp
 * @author 赵曦 (535394140@qq.com)
 * @brief Industrial Internet server based on OPC UA protocol
 * @version 1.0
 * @date 2022-09-15
 *
 * @copyright Copyright (c) 2023, zhaoxi
 *
 */

#include <thread>
#include <string>

#include "asmpro/opcua_cs/server.hpp"

using namespace std;
using namespace ua;

void Server::stopHandler(int sign)
{
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "received ctrl-c");
    Server::__running = false;
}

void Server::init(UA_UInt16 port, const vector<string> &user_name, const vector<string> &password)
{
    SERVER_RUNNING_ASSERT();
    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);

    __server = UA_Server_new();

    UA_ServerConfig *config = UA_Server_getConfig(__server);
    UA_ServerConfig_setMinimal(config, port, nullptr);

    if (!user_name.empty() && !password.empty() &&
        user_name.size() == password.size())
    {
        // Prepare the usr_passwd data
        vector<UA_UsernamePasswordLogin> usr_passwd;
        usr_passwd.reserve(user_name.size());
        for (size_t i = 0; i < user_name.size(); ++i)
        {
            UA_UsernamePasswordLogin each;
            each.username = UA_STRING(const_cast<char *>(user_name[i].c_str()));
            each.password = UA_STRING(const_cast<char *>(password[i].c_str()));
            usr_passwd.emplace_back(each);
        }
        // Configuration
        config->accessControl.clear(&config->accessControl);
        UA_AccessControl_default(config, false, nullptr,
                                 &config->securityPolicies[config->securityPoliciesSize - 1].policyUri,
                                 usr_passwd.size(), usr_passwd.data());
    }
    __is_init = UA_TRUE;
}

void Server::run()
{
    SERVER_RUNNING_ASSERT();
    SERVER_INIT_ASSERT();
    __running = true;
    UA_StatusCode retval = UA_Server_run(__server, &__running);
    if (retval != UA_STATUSCODE_GOOD)
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                     "ServerInit: %s", UA_StatusCode_name(retval));
}

UA_VariableAttributes Server::configVariableAttribute(const string &browse_name,
                                                      const string &description, const Variable &data)
{
    UA_VariableAttributes var_attr = UA_VariableAttributes_default;
    const UA_Variant &value = data.get();
    var_attr.value = value;
    var_attr.dataType = value.type->typeId;
    if (value.arrayLength == 0)
        var_attr.valueRank = UA_VALUERANK_SCALAR;
    else
    {
        var_attr.arrayDimensions = value.arrayDimensions;
        var_attr.arrayDimensionsSize = value.arrayDimensionsSize;
        var_attr.valueRank = 1;
    }

    var_attr.description = UA_LOCALIZEDTEXT(en_US, to_c(browse_name));
    var_attr.displayName = UA_LOCALIZEDTEXT(en_US, to_c(description));
    var_attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    return var_attr;
}

UA_NodeId Server::findNodeId(const UA_NodeId &origin_id, UA_UInt32 target_ns, const string &target_name)
{
    SERVER_INIT_ASSERT();
    auto qualified_name = UA_QUALIFIEDNAME(1, const_cast<char *>(target_name.c_str()));
    auto bpr = UA_Server_browseSimplifiedBrowsePath(__server, origin_id, target_ns, &qualified_name);
    UA_NodeId retval = UA_NODEID_NULL;
    if (bpr.statusCode != UA_STATUSCODE_GOOD || bpr.targetsSize < 1)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                     "Function findNodeId: %s \033[31m(ns = %u, name = %s)\033[0m",
                     UA_StatusCode_name(bpr.statusCode), target_ns, target_name.c_str());
        return UA_NODEID_NULL;
    }
    else
        UA_NodeId_copy(&bpr.targets[0].targetId.nodeId, &retval);
    return retval;
}

UA_NodeId Server::addVariableNode(const string &browse_name, const string &description,
                                  const Variable &data, const UA_NodeId &type_id)
{
    SERVER_INIT_ASSERT();
    auto var_attr = configVariableAttribute(browse_name, description, data);
    //!< Add the variable node to the information model
    UA_NodeId node_id;
    auto retval = UA_Server_addVariableNode(__server, UA_NODEID_NULL,
                                            UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                                            UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                                            UA_QUALIFIEDNAME(1, to_c(browse_name)),
                                            type_id, var_attr, nullptr, &node_id);
    if (retval != UA_STATUSCODE_GOOD)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                     "Function addVariableNode: %s", UA_StatusCode_name(retval));
        return UA_NODEID_NULL;
    }
    return node_id;
}

UA_Boolean Server::writeVariable(const UA_NodeId &node_id, const Variable &data)
{
    SERVER_INIT_ASSERT();
    auto status = UA_Server_writeValue(__server, node_id, data.get());
    if (status != UA_STATUSCODE_GOOD)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                     "Function writeVariable: %s", UA_StatusCode_name(status));
        return UA_FALSE;
    }
    return UA_TRUE;
}

UA_NodeId Server::createEvent(const UA_NodeId &event_type_id)
{
    SERVER_INIT_ASSERT();
    UA_NodeId event_id = UA_NODEID_NULL;
    auto status = UA_Server_createEvent(__server, event_type_id, &event_id);
    if (status != UA_STATUSCODE_GOOD)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                     "Function createEvent: %s", UA_StatusCode_name(status));
        return UA_NODEID_NULL;
    }
    return event_id;
}

UA_Boolean Server::writeProperty(const UA_NodeId &node_id, const std::string &target_name, const Variable &data)
{
    SERVER_INIT_ASSERT();
    auto retval = UA_Server_writeObjectProperty(__server, node_id,
                                                UA_QUALIFIEDNAME(0, to_c(target_name)),
                                                data.get());
    if (retval != UA_STATUSCODE_GOOD)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                     "Function writeProperty: %s \033[31m(qualified name: %s)\033[0m", UA_StatusCode_name(retval), target_name.c_str());
        return UA_FALSE;
    }
    return UA_TRUE;
}

UA_Boolean Server::triggerEvent(const UA_NodeId &node_id, const UA_NodeId &origin_id)
{
    SERVER_INIT_ASSERT();
    auto retval = UA_Server_triggerEvent(__server, node_id, origin_id, nullptr, UA_TRUE);
    if (retval != UA_STATUSCODE_GOOD)
    {
        UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                       "Triggering event failed: %s \033[31m(event id: ns=%u, s=%u)\033[0m",
                       UA_StatusCode_name(retval), node_id.namespaceIndex, node_id.identifier.numeric);
        return UA_FALSE;
    }
    return UA_TRUE;
}

Variable Server::readVariable(const UA_NodeId &node_id)
{
    SERVER_INIT_ASSERT();
    UA_Variant val;
    auto status = UA_Server_readValue(__server, node_id, &val);
    if (status != UA_STATUSCODE_GOOD)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                     "Function readVariable: %s", UA_StatusCode_name(status));
        return Variable();
    }
    // Variant information
    return Variable(val.data, val.type, val.arrayLength);
}

void Server::addVariableNodeValueCallBack(const UA_NodeId &node_id, ValueCallBackRead before_read,
                                          ValueCallBackWrite after_write)
{
    SERVER_RUNNING_ASSERT();
    SERVER_INIT_ASSERT();
    UA_ValueCallback callback;
    callback.onRead = before_read;
    callback.onWrite = after_write;
    auto status = UA_Server_setVariableNode_valueCallback(__server, node_id, callback);
    if (status != UA_STATUSCODE_GOOD)
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                     "Function addVariableNodeValueCallBack: %s", UA_StatusCode_name(status));
}

UA_NodeId Server::addDataSourceVariableNode(const string &browse_name, const string &description,
                                            const Variable &data, DataSourceRead on_read,
                                            DataSourceWrite on_write, const UA_NodeId &type_id)
{
    SERVER_INIT_ASSERT();
    auto var_attr = configVariableAttribute(browse_name, description, data);
    //!< Add the variable node to the information model
    UA_DataSource data_source;
    data_source.read = on_read;
    data_source.write = on_write;
    UA_NodeId node_id = UA_NODEID_NULL;
    auto retval = UA_Server_addDataSourceVariableNode(__server, UA_NODEID_NULL,
                                                      UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                                                      UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                                                      UA_QUALIFIEDNAME(1, to_c(browse_name)),
                                                      type_id, var_attr, data_source, nullptr, &node_id);
    if (retval != UA_STATUSCODE_GOOD)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                     "Function addDataSourceVariableNode: %s", UA_StatusCode_name(retval));
        return UA_NODEID_NULL;
    }
    return node_id;
}

void Server::createVariableMonitor(UA_NodeId &node_id, UA_Server_DataChangeNotificationCallback data_change,
                                   UA_Double sampling_interval)
{
    SERVER_INIT_ASSERT();
    UA_MonitoredItemCreateRequest mon_request = UA_MonitoredItemCreateRequest_default(node_id);
    //!< Sampling interval (ms)
    mon_request.requestedParameters.samplingInterval = sampling_interval;
    UA_MonitoredItemCreateResult mon_response =
        UA_Server_createDataChangeMonitoredItem(__server, UA_TIMESTAMPSTORETURN_BOTH,
                                                mon_request, &node_id, data_change);
    if (mon_response.statusCode != UA_STATUSCODE_GOOD)
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                     "Function createVariableMonitor: %s", UA_StatusCode_name(mon_response.statusCode));
}

UA_NodeId Server::addVariableTypeNode(const string &browse_name, const string &description,
                                      const VariableType &data)
{
    SERVER_INIT_ASSERT();
    UA_VariableTypeAttributes type_attr = UA_VariableTypeAttributes_default;
    const UA_Variant &val = data.get();
    type_attr.value = val;
    type_attr.dataType = val.type->typeId;
    if (val.arrayLength == 0)
        type_attr.valueRank = UA_VALUERANK_SCALAR;
    else
    {
        type_attr.arrayDimensions = val.arrayDimensions;
        type_attr.arrayDimensionsSize = val.arrayDimensionsSize;
        type_attr.valueRank = 1;
    }
    type_attr.displayName = UA_LOCALIZEDTEXT(en_US, to_c(browse_name));
    type_attr.description = UA_LOCALIZEDTEXT(en_US, to_c(description));
    UA_NodeId node_id;
    auto retval = UA_Server_addVariableTypeNode(__server, UA_NODEID_NULL,
                                                UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                                                UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
                                                UA_QUALIFIEDNAME(1, to_c(browse_name)),
                                                UA_NODEID_NULL, type_attr, nullptr, &node_id);
    if (retval != UA_STATUSCODE_GOOD)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                     "Function addVariableTypeNode: %s", UA_StatusCode_name(retval));
        return UA_NODEID_NULL;
    }
    return node_id;
}

UA_NodeId Server::addObjectNode(const string &browse_name, const string &description, const Object &data,
                                const UA_NodeId &type_id, const UA_NodeId &parent_id)
{
    SERVER_INIT_ASSERT();
    UA_ObjectAttributes obj_attr = UA_ObjectAttributes_default;
    obj_attr.displayName = UA_LOCALIZEDTEXT(en_US, to_c(browse_name));
    obj_attr.description = UA_LOCALIZEDTEXT(en_US, to_c(description));
    //!< Object NodeId
    UA_NodeId node_id = UA_NODEID_NULL;
    auto retval = UA_Server_addObjectNode(__server, UA_NODEID_NULL, parent_id,
                                          UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                                          UA_QUALIFIEDNAME(1, to_c(browse_name)),
                                          type_id, obj_attr, nullptr, &node_id);
    if (retval != UA_STATUSCODE_GOOD)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                     "Function addObject: %s", UA_StatusCode_name(retval));
        return UA_NODEID_NULL;
    }
    const auto &name_vars = data.get();
    for (const auto &[name, variable] : name_vars)
    {
        auto var_node_id = findNodeId(node_id, 1, name);
        //!< Set variable
        auto write_static = UA_Server_writeValue(__server, var_node_id, variable.second.get());
        if (write_static != UA_STATUSCODE_GOOD)
        {
            UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                         "Function addObject, write variable: %s", UA_StatusCode_name(retval));
            return UA_NODEID_NULL;
        }
    }
    return node_id;
}

UA_NodeId Server::addObjectTypeNode(const string &browse_name, const string &description,
                                    const ObjectType &data, UA_NodeId parent_id)
{
    SERVER_INIT_ASSERT();
    //!< Define the object type node
    UA_ObjectTypeAttributes obj_attr = UA_ObjectTypeAttributes_default;
    obj_attr.displayName = UA_LOCALIZEDTEXT(en_US, to_c(browse_name));
    obj_attr.description = UA_LOCALIZEDTEXT(en_US, to_c(description));
    UA_NodeId node_id = UA_NODEID_NULL;
    auto retval = UA_Server_addObjectTypeNode(__server, UA_NODEID_NULL, parent_id,
                                              UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
                                              UA_QUALIFIEDNAME(1, to_c(browse_name)),
                                              obj_attr, nullptr, &node_id);
    if (retval != UA_STATUSCODE_GOOD)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                     "Function addObjectType: %s", UA_StatusCode_name(retval));
        return UA_NODEID_NULL;
    }
    //!< Add variable node to object type as child node
    const auto &name_vars = data.get();
    for (const auto &[name, variable] : name_vars)
    {
        //!< Add variable attributes
        UA_VariableAttributes attr = configVariableAttribute(name, "", variable.second);
        UA_NodeId var_node_id = UA_NODEID_NULL;
        retval = UA_Server_addVariableNode(__server, UA_NODEID_NULL, node_id,
                                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                                           UA_QUALIFIEDNAME(1, to_c(name)),
                                           variable.first, attr, nullptr, &var_node_id);
        if (retval != UA_STATUSCODE_GOOD)
        {
            UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                         "addObjectTypeNode->addVariableNode: %s\033[31m(name = %s)\033[0m",
                         UA_StatusCode_name(retval), name.c_str());
            return UA_NODEID_NULL;
        }
        // Make the name mandatory
        retval = UA_Server_addReference(__server, var_node_id, UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                                        UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);
        if (retval != UA_STATUSCODE_GOOD)
        {
            UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                         "addObjectTypeNode->addReference: %s", UA_StatusCode_name(retval));
            return UA_NODEID_NULL;
        }
    }
    return node_id;
}

UA_NodeId Server::addEventTypeNode(const string &browse_name, const string &description,
                                   const EventType &data)
{
    SERVER_INIT_ASSERT();
    UA_ObjectTypeAttributes attr = UA_ObjectTypeAttributes_default;
    attr.displayName = UA_LOCALIZEDTEXT(en_US, to_c(browse_name));
    attr.description = UA_LOCALIZEDTEXT(en_US, to_c(description));
    UA_NodeId event_type_node_id = UA_NODEID_NULL;
    auto retval = UA_Server_addObjectTypeNode(__server, UA_NODEID_NULL,
                                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEEVENTTYPE),
                                              UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
                                              UA_QUALIFIEDNAME(1, to_c(browse_name)),
                                              attr, nullptr, &event_type_node_id);
    if (retval != UA_STATUSCODE_GOOD)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                     "Function addEventType: %s", UA_StatusCode_name(retval));
        return UA_NODEID_NULL;
    }
    //!< Add variable node to object type as child node
    const auto &name_vars = data.get();
    for (const auto &[name, variable] : name_vars)
    {
        //!< Add variable attributes
        UA_VariableAttributes attr = UA_VariableAttributes_default;
        attr.displayName = UA_LOCALIZEDTEXT(en_US, to_c(name));
        attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
        UA_Variant value = variable.second.get();
        attr.value = value;
        UA_NodeId var_node_id;
        retval = UA_Server_addVariableNode(__server, UA_NODEID_NULL, event_type_node_id,
                                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                                           UA_QUALIFIEDNAME(0, to_c(name)),
                                           UA_NODEID_NUMERIC(0, UA_NS0ID_PROPERTYTYPE),
                                           attr, nullptr, &var_node_id);
        if (retval != UA_STATUSCODE_GOOD)
        {
            UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                         "Function addEventType, add variable: %s", UA_StatusCode_name(retval));
            return UA_NODEID_NULL;
        }
        // Make the name mandatory
        retval = UA_Server_addReference(__server, var_node_id,
                                        UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                                        UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);
        if (retval != UA_STATUSCODE_GOOD)
        {
            UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                         "AddEventTypeVariable: %s", UA_StatusCode_name(retval));
            return UA_NODEID_NULL;
        }
    }
    return event_type_node_id;
}

UA_NodeId Server::addMethodNode(const string &browse_name, const string &description, UA_MethodCallback on_method,
                                const vector<Argument> &input_args, const vector<Argument> &output_args,
                                const UA_NodeId &parent_id)
{
    SERVER_INIT_ASSERT();
    UA_MethodAttributes method_attr = UA_MethodAttributes_default;
    method_attr.displayName = UA_LOCALIZEDTEXT(en_US, to_c(browse_name));
    method_attr.description = UA_LOCALIZEDTEXT(en_US, to_c(description));
    method_attr.executable = true;
    method_attr.userExecutable = true;
    //!< Extract core data: UA_Argument
    vector<UA_Argument> inputs;
    inputs.reserve(input_args.size());
    for (auto &inputArg : input_args)
        inputs.emplace_back(inputArg.get());
    vector<UA_Argument> outputs;
    outputs.reserve(output_args.size());
    for (auto &outputArg : output_args)
        outputs.emplace_back(outputArg.get());
    //!< Add method node
    UA_NodeId node_id = UA_NODEID_NULL;
    auto retval = UA_Server_addMethodNode(__server, UA_NODEID_NULL, parent_id,
                                          UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                                          UA_QUALIFIEDNAME(1, to_c(browse_name)),
                                          method_attr, on_method, input_args.size(), inputs.data(),
                                          output_args.size(), outputs.data(), nullptr, &node_id);
    if (retval != UA_STATUSCODE_GOOD)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                     "Function addMethod: %s", UA_StatusCode_name(retval));
        return UA_NODEID_NULL;
    }
    // Make the name mandatory
    retval = UA_Server_addReference(__server, node_id,
                                    UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                                    UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);
    if (retval != UA_STATUSCODE_GOOD)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                     "AddEventTypeVariable: %s", UA_StatusCode_name(retval));
        return UA_NODEID_NULL;
    }
    return node_id;
}
