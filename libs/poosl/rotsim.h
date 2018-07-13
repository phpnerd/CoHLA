/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <iostream>
#include <sstream>
#include "rotalumisrequest.hxx"
#include "rotalumisresponse.hxx"
#include <chrono>
#include <socket.h>
#ifdef LINUX
#include <unistd.h>
#endif
#ifdef WINDOWS
#include <windows.h>
#endif

#define PROFILE_TARGET false
#define PROFILE_LENGTH 3600

typedef unsigned char byte;

using namespace std;
using namespace poosl;
using namespace xercesc;
namespace sc = std::chrono;

class RotalumisSimulator {

public:
    RotalumisSimulator(string hostname, uint16_t port, string pooslModel);
    virtual ~RotalumisSimulator() = default;
    virtual void updateAttributeValues() = 0;
    double proceed();
    void advanceTime(long double time);
    long double getTime();
    set<string> getChangedAttributes();

protected:
    int requestId;
    bool initiated = false;

    set<string> changedAttributes;

    void connect();
    void init();

    T_transition_list::TransitionSequence getTransitions();
    T_inspect_process getProcess(string processName);
    T_variable getElement(T_inspect_process process, string variableName);
    void setElement(T_inspect_process process, string& processName, string& variableName, int variableValue);
    void setElement(T_inspect_process process, string& processName, string& variableName, double variableValue);
    void setElement(T_inspect_process process, string& processName, string& variableName, float variableValue);
    void setElement(T_inspect_process process, string& processName, string& variableName, bool variableValue);
    void setElement(T_inspect_process process, string& processName, string& variableName, string variableValue);


    Response send(const Request request, function<bool(Response)> filter);
    void send(const Request request);
    void send(const string str);
    Response awaitResponse(function<bool(Response)>);
    Response nextResponse();
    u_int responseLength();
    void initModel();

    int rid();

    static std::wstring toWString(std::string str) {
        std::wstring str2(str.length(), L' ');
        std::copy(str.begin(), str.end(), str2.begin());
        return str2;
    }

private:
    string hostname;
    string model;
    uint16_t port;

    Socket* socket;

    long double rotTime, simTime;
    map<string, map<string, pair<long long, long long>>> varHandles;

    Request timeStepRequest, stepRequest;
    Response respBuffer;

    T_variable cVar(T_inspect_process process, string& processName, string& variableName);
    void setElement(T_inspect_process process, string processName, T_variable variable);
    void setHandles(T_inspect_process process, string processName, string variableName);
    T_execution_state_change_response awaitExecutionState();
    T_execution_state_change_response getExecutionState();
    Request createTimeStepRequest();
    Request createStepRequest();
    Request getTsr();

    void sleep(const u_int sleepMs) {
#ifdef LINUX
        usleep(sleepMs * 1000);
#endif
#ifdef WINDOWS
        Sleep(sleepMs);
#endif
    }

#if PROFILE_TARGET
    map<string, pair<int, long long>> table;
    void startTime(string label);
    void stopTime();
    void printTable();
    long long getProfilerTime();
    string tl[20];
    long long ts[20];
    int ti = 0;
#endif

};