/**
 * @file delaycal_client.cpp
 * @author zhaoxi (535394140@qq.com)
 * @brief
 * @version 1.0
 * @date 2023-02-25
 *
 * @copyright Copyright (c) 2023, zhaoxi
 *
 */

#include <iostream>

#include "asmpro/opcua_cs/client.hpp"

using namespace std;
using namespace ua;

uint64_t toMicroSec(UA_DateTimeStruct &s)
{
    uint64_t microSec = s.microSec;
    uint64_t milliSec = s.milliSec;
    uint64_t sec = s.sec;
    uint64_t min = s.min;
    uint64_t hour = s.hour;

    return microSec + 1000U * milliSec + 1000000U * sec + 60000000U * min + 3600000000U * hour;
}

int main(int argc, char *argv[])
{
    uint64_t time_100 = 0;
    for (int i = 0; i < 100; ++i)
    {
        Client client;
        client.connect("opc.tcp://localhost:4850");
        UA_NodeId time_tick_id = client.findNodeId(UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER), 1, "DelayCalculate");
        vector<UA_Byte> data(1280 * 960 * 3);
        vector<Variable> input;
        input.emplace_back(data.data(), &UA_TYPES[UA_TYPES_BYTE], data.size());
        vector<Variable> output;
        UA_DateTime before = UA_DateTime_now();
        client.call(time_tick_id, input, output);
        client.runIterate(0);
        UA_DateTime now = UA_DateTime_now();
        UA_DateTimeStruct before_struct = UA_DateTime_toStruct(before);
        UA_DateTimeStruct now_struct = UA_DateTime_toStruct(now);
        uint64_t time = toMicroSec(now_struct) - toMicroSec(before_struct);
        cout << "\033[35mtime = " << time << " μs\033[0m" << endl;
        time_100 += time;
    }
    cout << "avg time = " << time_100 / 200.0 << endl;
    return 0;
}
