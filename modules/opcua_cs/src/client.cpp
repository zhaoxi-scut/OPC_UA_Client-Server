/**
 * @file client.cpp
 * @author 赵曦 (535394140@qq.com)
 * @brief Industrial Internet server based on OPC UA protocol
 * @version 1.0
 * @date 2022-09-22
 *
 * @copyright Copyright (c) 2023, zhaoxi
 *
 */

#include <string>
#include <memory>

#include "asmpro/opcua_cs/client.hpp"

using namespace std;
using namespace ua;

Client::Client()
{
    __client = UA_Client_new();
    UA_ClientConfig *config = UA_Client_getConfig(__client);
    UA_StatusCode status = UA_ClientConfig_setDefault(config);
    if (status != UA_STATUSCODE_GOOD)
        UA_Client_delete(__client);
}

UA_Boolean Client::connect(const string &address, const string &username, const string &password)
{
    if (username.empty() || password.empty())
        return UA_Client_connect(__client, address.c_str()) == UA_STATUSCODE_GOOD;
    __is_connect = UA_Client_connectUsername(__client, address.c_str(), username.c_str(), password.c_str()) == UA_STATUSCODE_GOOD;
    return __is_connect;
}

UA_NodeId Client::findNodeId(const UA_NodeId &origin_id, const UA_UInt32 target_ns, const string &target_name)
{
    UA_BrowsePath browsePath;
    UA_BrowsePath_init(&browsePath);
    browsePath.startingNode = origin_id;
    unique_ptr<UA_RelativePathElement> elem = make_unique<UA_RelativePathElement>();
    browsePath.relativePath.elements = elem.get();
    browsePath.relativePath.elementsSize = 1;

    elem->targetName = UA_QUALIFIEDNAME(target_ns, to_c(target_name));

    UA_TranslateBrowsePathsToNodeIdsRequest request;
    UA_TranslateBrowsePathsToNodeIdsRequest_init(&request);
    request.browsePaths = &browsePath;
    request.browsePathsSize = 1;

    UA_TranslateBrowsePathsToNodeIdsResponse response = UA_Client_Service_translateBrowsePathsToNodeIds(__client, request);
    if (response.responseHeader.serviceResult == UA_STATUSCODE_GOOD)
        if (response.resultsSize == 1 && response.results[0].targetsSize == 1)
            return response.results[0].targets[0].targetId.nodeId;

    printf("Error: %s\n", UA_StatusCode_name(response.responseHeader.serviceResult));
    return UA_NODEID_NULL;
}

UA_Boolean Client::writeVariable(const UA_NodeId &node_id, const Variable &data)
{
    auto status = UA_Client_writeValueAttribute(__client, node_id, &data.get());
    if (status != UA_STATUSCODE_GOOD)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_CLIENT, "%s", UA_StatusCode_name(status));
        return UA_FALSE;
    }
    return UA_TRUE;
}

Variable Client::readVariable(const UA_NodeId &node_id)
{
    UA_Variant val;
    UA_Variant_init(&val);
    UA_StatusCode retval = UA_Client_readValueAttribute(__client, node_id, &val);
    if (retval != UA_STATUSCODE_GOOD)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_CLIENT, "%s", UA_StatusCode_name(retval));
        return Variable();
    }
    //!< Variant information
    return Variable(val.data, val.type, val.arrayLength);
}

UA_Boolean Client::call(const UA_NodeId &node_id, const std::vector<Variable> &inputs,
                        std::vector<Variable> &outputs, const UA_NodeId &parent_id)
{
    //!< Initialize the I/O argumenst: vector of UA_Variant
    vector<UA_Variant> input_variants;
    input_variants.reserve(inputs.size());
    for (const auto &input : inputs)
        input_variants.push_back(input.get());

    size_t output_size;
    UA_Variant *output_variants;
    //!< Call
    UA_StatusCode retval = UA_Client_call(__client, parent_id, node_id,
                                          input_variants.size(), input_variants.data(),
                                          &output_size, &output_variants);
    if (retval != UA_STATUSCODE_GOOD)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_CLIENT, "Failed to call the method: %s \033[31m(ns=%u,s=%d)\033[0m",
                     UA_StatusCode_name(retval), node_id.namespaceIndex, node_id.identifier.numeric);
        return UA_FALSE;
    }    
    //!< Process the output variants
    outputs.reserve(output_size);
    for (size_t i = 0; i < output_size; ++i)
        outputs.emplace_back(output_variants[i].data, output_variants[i].type, output_variants[i].arrayLength);
    return UA_TRUE;
}

UA_UInt32 Client::createSubscription()
{
    UA_CreateSubscriptionRequest sub_request = UA_CreateSubscriptionRequest_default();
    UA_CreateSubscriptionResponse sub_response =
        UA_Client_Subscriptions_create(__client, sub_request, nullptr, nullptr, nullptr);
    UA_UInt32 sub_id = 0;
    if (sub_response.responseHeader.serviceResult == UA_STATUSCODE_GOOD)
    {
        sub_id = sub_response.subscriptionId;
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_CLIENT,
                    "\033[32m[subscription id: %u] Create subscription succeeded!\033[0m", sub_id);
    }
    else
        UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_CLIENT, "\033[33mFailed to create subscription!\033[0m");
    return sub_id;
}

UA_Boolean Client::createVariableMonitor(UA_UInt32 sub_id, UA_NodeId node_id,
                                         UA_Client_DataChangeNotificationCallback data_change_handler)
{
    UA_MonitoredItemCreateRequest request_item = UA_MonitoredItemCreateRequest_default(node_id);
    UA_MonitoredItemCreateResult result =
        UA_Client_MonitoredItems_createDataChange(__client, sub_id, UA_TIMESTAMPSTORETURN_BOTH,
                                                  request_item, &node_id, data_change_handler, nullptr);
    if (result.statusCode != UA_STATUSCODE_GOOD)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_CLIENT,
                     "\033[31mcreateVariableMonitor: %s\033[0m", UA_StatusCode_name(result.statusCode));
        return UA_FALSE;
    }
    else
    {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_CLIENT,
                    "\033[32m[subscription id: %u, monitoredItem id: %u] Monitoring the node: ns=%u, s=%u\033[0m",
                    sub_id, result.monitoredItemId, node_id.identifier.numeric, node_id.namespaceIndex);
        return UA_TRUE;
    }
}

UA_Boolean Client::createEventMonitor(UA_UInt32 sub_id, UA_NodeId node_id, vector<string> &names,
                                      UA_Client_EventNotificationCallback event_handler)
{
    UA_MonitoredItemCreateRequest request_item;
    UA_MonitoredItemCreateRequest_init(&request_item);
    request_item.itemToMonitor.nodeId = node_id;
    request_item.itemToMonitor.attributeId = UA_ATTRIBUTEID_EVENTNOTIFIER;
    request_item.monitoringMode = UA_MONITORINGMODE_REPORTING;
    // Prepare the 'UA_QualifiedName'
    vector<UA_QualifiedName> qualified_names(names.size());
    for (size_t i = 0; i < qualified_names.size(); ++i)
        qualified_names[i] = UA_QUALIFIEDNAME(0, to_c(names[i]));
    // Prepare the 'select_clauses'
    vector<UA_SimpleAttributeOperand> select_clauses;
    select_clauses.resize(qualified_names.size());
    for (size_t i = 0; i < qualified_names.size(); ++i)
    {
        select_clauses[i].typeDefinitionId = UA_NODEID_NUMERIC(0, UA_NS0ID_BASEEVENTTYPE);
        select_clauses[i].attributeId = UA_ATTRIBUTEID_VALUE;
        select_clauses[i].browsePathSize = 1;
        select_clauses[i].browsePath = &qualified_names[i];
    }
    // Prepare the EventFilter
    UA_EventFilter filter;
    UA_EventFilter_init(&filter);
    filter.selectClauses = select_clauses.data();
    filter.selectClausesSize = select_clauses.size();
    request_item.requestedParameters.filter.encoding = UA_EXTENSIONOBJECT_DECODED;
    request_item.requestedParameters.filter.content.decoded.data = &filter;
    request_item.requestedParameters.filter.content.decoded.type = &UA_TYPES[UA_TYPES_EVENTFILTER];
    UA_UInt32 monitor_id = 0;
    UA_MonitoredItemCreateResult result =
        UA_Client_MonitoredItems_createEvent(__client, sub_id, UA_TIMESTAMPSTORETURN_BOTH,
                                             request_item, &monitor_id, event_handler, nullptr);
    if (result.statusCode != UA_STATUSCODE_GOOD)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_CLIENT,
                     "\033[31mcreateEventMonitor: %s\033[0m", UA_StatusCode_name(result.statusCode));
        return UA_FALSE;
    }
    else
    {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_CLIENT,
                    "\033[32m[subscription id: %u, monitoredItem id: %u] Monitoring the node: ns=%u, s=%u\033[0m",
                    sub_id, result.monitoredItemId, node_id.identifier.numeric, node_id.namespaceIndex);
        return UA_TRUE;
    }
}
