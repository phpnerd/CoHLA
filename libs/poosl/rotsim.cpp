/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <rotsim.h>

RotalumisSimulator::RotalumisSimulator(string hostname, uint16_t port, string pooslModel):
    hostname(hostname),
    port(port),
    requestId(1),
    rotTime(0.0),
    simTime(0.0),
    model(pooslModel),
    socket(new Socket(hostname, port))
{
    this->connect();
    this->init();
}

void RotalumisSimulator::init() {
    initModel();
    timeStepRequest = createTimeStepRequest();
    stepRequest = createStepRequest();
}

void RotalumisSimulator::connect() {
    wcout << "Connecting to Rotalumis instance on " << this->hostname.c_str() << ":" << this->port << endl;
    socket->connect(false);
    wcout << "Connected to Rotalumis" << endl;
}

void RotalumisSimulator::initModel() {
    XMLPlatformUtils::Initialize();
    Request req(rid());
    T_compile_request cr;
    cr.setPoosl_specification(model);
    req.setCompile(cr);
    Response resp = send(req, [](Response r) { return r.getCompile().present(); });
    if (resp.getCompile().get().getError().length() != 0) {
        wcerr << "Error while initialising model: " << resp.getCompile().get().getError() << endl;
        return;
    }
    T_compile_response::HandleType modelHandle = resp.getCompile().get().getHandle();

    req = Request(rid());
    T_instantiate_request tir;
    tir.setPoosl_specification(modelHandle);
    req.setInstantiate(tir);
    if (send(req, [](Response r) { return r.getInstantiate().present(); }).getInstantiate().get().getResult() != "ok") {
        wcerr << "Instantiation failed" << endl;
        return;
    }

    req = Request(rid());
    T_command_request tcr;
    tcr.setType(T_command::pause);
    req.setCommand(tcr);
    if (send(req, [](Response r) { return r.getCommand().present() && r.getCommand().get().getResult().present(); }).getCommand().get().getResult().get() != "ok") {
        wcerr << "Failed to pause" << endl;
        return;
    }

    req = Request(rid());
    T_eengine_event_setup_request teesr;
    teesr.setCommunication_messages_enable(true);
    teesr.setCommunication_messages_credits_enable(false);
    teesr.setCommunication_messages_credits(0);
    req.setEengine_event_setup(teesr);
    if (send(req, [](Response r) { return r.getEengine_event_setup().present(); }).getEengine_event_setup().get().getResult() != "ok") {
        wcerr << "Failed to enable communication messages" << endl;
        return;
    }

    req = Request(rid());
    req.setExecution_state(T_execution_state_request());
    if (send(req, [](Response r) { return r.getExecution_state().present(); }).getExecution_state().get().getTime() != 0) {
        wcerr << "Failed to create execution state" << endl;
        return;
    }

    T_transition_list::TransitionSequence transitions = getTransitions();

    this->initiated = !transitions.empty();
    if (!this->initiated)
        wcerr << "Failed to initialise model" << endl;
    else
        wcout << "Model initialised" << endl;
}

void RotalumisSimulator::advanceTime(long double time) {
    this->simTime = time;
#if PROFILE_TARGET
    if (time > PROFILE_LENGTH) {
        stopTime();
        printTable();
        while(true);
    }
#endif
}

double RotalumisSimulator::proceed() {
#if PROFILE_TARGET
    if (time == 0)
        startTime("Sim");
    startTime("AdvanceTime");
#endif
    if (send(getTsr(), [](Response r) { return r.getCommand().present() && r.getCommand().get().getResult().present(); }).getCommand().get().getResult().get() != "ok") {
        wcerr << "Failed to proceed in time!";
#if PROFILE_TARGET
        stopTime();
#endif
        return 0;
    }
#if PROFILE_TARGET
    stopTime();
    startTime("AwaitExecutionState");
#endif
    T_execution_state_change_response state = awaitExecutionState();
#if PROFILE_TARGET
    stopTime();
#endif
    double stepSize = std::numeric_limits<double>::max();
    if (state.getTime() > rotTime)
        stepSize = state.getTime() - (double)rotTime;
    rotTime = state.getTime();
    if (state.getTime() > simTime)
        stepSize = state.getTime() - (double)simTime;
    return stepSize;
}

T_execution_state_change_response RotalumisSimulator::awaitExecutionState() {
    return awaitResponse([](Response r) {
        return r.getExecution_state().present() && r.getExecution_state().get().getState() == "paused";
    }).getExecution_state().get();
}

T_execution_state_change_response RotalumisSimulator::getExecutionState() {
    Request req(rid());
    req.setExecution_state(T_execution_state_request());
    return send(req, [](Response r) { return r.getExecution_state().present(); }).getExecution_state().get();
}

T_transition_list::TransitionSequence RotalumisSimulator::getTransitions() {
    Request req(rid());
    req.setGet_transitions(T_get_transitions_request());
    return send(req, [](Response r) { return r.getGet_transitions().present(); }).getGet_transitions().get().getTransitions().getTransition();
}

T_inspect_process RotalumisSimulator::getProcess(string processName) {
#if PROFILE_TARGET
    startTime("GetProcess");
#endif
    Request req(rid());
    T_inspect_request tir;
    tir.setName("/system/" + processName);
    tir.setType(T_inspect_type::process);
    req.setInspect(tir);
    Response resp = send(req, [](Response r) { return r.getInspect().present(); });

    if (resp.getInspect().get().getResult() != T_inspect_response::ResultType::ok || !resp.getInspect().get().getProcess().present()) {
#if PROFILE_TARGET
        stopTime();
#endif
        return T_inspect_process();
    }
    T_inspect_process tip = resp.getInspect().get().getProcess().get();
#if PROFILE_TARGET
    stopTime();
#endif
    return tip;
}

T_variable RotalumisSimulator::getElement(T_inspect_process process, string variableName) {
    T_var_list::VariableSequence vars = process.getInstance_variables().getVariable();
    for (auto &var : vars)
        if (var.getName() == variableName)
            return var;
    return T_variable();
}

void RotalumisSimulator::setElement(T_inspect_process process, string& processName, string& variableName, int variableValue) {
    T_variable var = cVar(process, processName, variableName);
    var.setLiteral(to_string(variableValue));
    var.setType(T_constant_type(T_constant_type::integer));
    setElement(process, processName, var);
}

void RotalumisSimulator::setElement(T_inspect_process process, string& processName, string& variableName, double variableValue) {
    T_variable var = cVar(process, processName, variableName);
    var.setLiteral(to_string(variableValue));
    var.setType(T_constant_type(T_constant_type::real));
    setElement(process, processName, var);
}

void RotalumisSimulator::setElement(T_inspect_process process, string& processName, string& variableName, float variableValue) {
    T_variable var = cVar(process, processName, variableName);
    var.setLiteral(to_string(variableValue));
    var.setType(T_constant_type(T_constant_type::real));
    setElement(process, processName, var);
}

void RotalumisSimulator::setElement(T_inspect_process process, string& processName, string& variableName, bool variableValue) {
    T_variable var = cVar(process, processName, variableName);
    var.setLiteral(variableValue ? "true" : "false");
    var.setType(T_constant_type(T_constant_type::boolean));
    setElement(process, processName, var);
}

void RotalumisSimulator::setElement(T_inspect_process process, string& processName, string& variableName, string variableValue) {
    T_variable var = cVar(process, processName, variableName);
    var.setLiteral(variableValue);
    var.setType(T_constant_type(T_constant_type::string));
    setElement(process, processName, var);
}

T_variable RotalumisSimulator::cVar(T_inspect_process process, string& processName, string& variableName) {
    T_variable var;
    var.setName(variableName);
    if (varHandles.find(processName) == varHandles.end() || varHandles[processName].find(variableName) == varHandles[processName].end())
        setHandles(process, processName, variableName);
    var.setHandle(varHandles[processName][variableName].first);
    return var;
}

void RotalumisSimulator::setElement(T_inspect_process process, string processName, T_variable variable) {
#if PROFILE_TARGET
    startTime("SetElement");
#endif
    Request req(rid());
    T_set_variable_request tsvr;
    if (varHandles.find(processName) == varHandles.end() || varHandles[processName].find(variable.getName()) == varHandles[processName].end())
        setHandles(process, processName, variable.getName());
    tsvr.setList_handle(varHandles[processName][variable.getName()].second);
    tsvr.setVar_handle(variable.getHandle());
    tsvr.setType(variable.getType());
    tsvr.setLiteral(variable.getLiteral());
    req.setSet_variable(tsvr);
    if (send(req, [](Response r) { return r.getSet_variable().present(); }).getSet_variable().get().getResult() != "ok")
        wcerr << "Failed to set variable " << variable.getName().c_str() << endl;
#if PROFILE_TARGET
    stopTime();
#endif
}

void RotalumisSimulator::setHandles(T_inspect_process process, string processName, string variableName) {
    T_var_list::VariableSequence variables = process.getInstance_variables().getVariable();
    T_variable var = T_variable();
    for (auto variable : variables)
        if (variable.getName() == variableName)
            var = variable;
    if (var.getName().empty())
        return;
    long long listHandle = 0;
    if (!process.getExecution_tree().getSequential().empty() && process.getExecution_tree().getSequential()[0].getGlobal().present())
        listHandle = process.getExecution_tree().getSequential()[0].getGlobal().get();
    else if (!process.getExecution_tree().getMethod_call().empty() && process.getExecution_tree().getMethod_call()[0].getGlobal().present())
        listHandle = process.getExecution_tree().getMethod_call()[0].getGlobal().get();
    else if (!process.getExecution_tree().getParallel().empty() && process.getExecution_tree().getParallel()[0].getGlobal().present())
        listHandle = process.getExecution_tree().getParallel()[0].getGlobal().get();
    varHandles[processName][variableName].first = var.getHandle();
    varHandles[processName][variableName].second = listHandle;
    wcout << "Set handles for " << toWString(variableName) << endl;
}

Request RotalumisSimulator::createTimeStepRequest() {
    Request req(0);
    T_command_request tcr;
    tcr.setType(T_command::time_step);
    req.setCommand(tcr);
    return req;
}

Request RotalumisSimulator::createStepRequest() {
    Request req(0);
    T_command_request tcr;
    tcr.setType(T_command::step);
    req.setCommand(tcr);
    return req;
}

Request RotalumisSimulator::getTsr() {
    Request req = timeStepRequest;
    req.setId(rid());
    return req;
}

void RotalumisSimulator::send(const Request request) {
    ostringstream oss("");
    xml_schema::NamespaceInfomap map;
    map[""].name = "uri:poosl";
    serializeRequest(oss, request, map, "ISO-8859-1", xml_schema::Flags::dont_pretty_print);
    send(oss.str());
}

Response RotalumisSimulator::send(const Request request, function<bool(Response)> filter) {
    send(request);
    return awaitResponse(filter);
}

void RotalumisSimulator::send(const string str) {
    unsigned long length = 12 + str.length();
    auto s = new char[length+1];
    snprintf(s, length+1, "ROT %08x%s\n", (u_int)str.length(), str.c_str());
//    wcout << L"Sending: " << s << endl;
    socket->write(s);
}

Response RotalumisSimulator::awaitResponse(function<bool(Response)> filter) {
#if PROFILE_TARGET
    startTime("AwaitResponse");
#endif
    if (filter(respBuffer)) {
        Response r = respBuffer;
        respBuffer = Response();
#if PROFILE_TARGET
        stopTime();
#endif
        return r;
    }
    Response r;
    while (!filter((r = nextResponse())))
        respBuffer = r;
#if PROFILE_TARGET
    stopTime();
#endif
    return r;
}

Response RotalumisSimulator::nextResponse() {
#if PROFILE_TARGET
    startTime("NextResponse");
#endif
    u_int length = responseLength();
    auto resp = (byte*)malloc(sizeof(byte) * length);
    size_t length_r = socket->read(resp, length);
    if (length != length_r)
        wcerr << "Invalid length read!" << endl;
//    wcout << L"Received: " << string(resp, resp + length * sizeof(unsigned char)).c_str() << endl;
    istringstream iss(string(resp, resp + length * sizeof(unsigned char)));
    xml_schema::Properties properties;
    properties.schema_location("uri:poosl", "file:///home/thomas/RU/Research/workspace/OpenRTIExample/res/rotalumisresponse.xsd");
    unique_ptr<Response> r = parseResponse(iss, xml_schema::Flags::dont_validate, properties);
#if PROFILE_TARGET
    stopTime();
#endif
    return r.operator*();
}

u_int RotalumisSimulator::responseLength() {
#if PROFILE_TARGET
    startTime("ResponseLength");
    startTime("BytesAvailable");
#endif
    while(socket->bytesAvailable() < 12)
#if PROFILE_TARGET
            stopTime();
            stopTime();
#endif
        sleep(1);
#if PROFILE_TARGET
    stopTime();
    startTime("ReadLen");
#endif
    byte lengthHeader[4];
    socket->read(lengthHeader, 4);
    if (lengthHeader[0] != 'R' || lengthHeader[1] != 'O' || lengthHeader[2] != 'T' || lengthHeader[3] != ' ') {
        wcerr << L"Invalid length header received! (" << lengthHeader << ")" << endl;
#if PROFILE_TARGET
        stopTime();
#endif
        return responseLength();
    }
    byte lengthValue[8];
    socket->read(lengthValue, 8);
    u_int length;
    stringstream ss;
    ss << hex << lengthValue;
    ss >> length;
#if PROFILE_TARGET
    stopTime();
    stopTime();
#endif
    return length;
}

long double RotalumisSimulator::getTime() {
    return simTime;
}

set<string> RotalumisSimulator::getChangedAttributes() {
    set<string> ca(changedAttributes);
    changedAttributes.clear();
    return ca;
}

#if PROFILE_TARGET
void RotalumisSimulator::startTime(string label) {
    ts[ti] = getProfilerTime();
    tl[ti++] = label;
}

void RotalumisSimulator::stopTime() {
    ti--;
    table[tl[ti]].first++;
    table[tl[ti]].second += getProfilerTime() - ts[ti];
}

void RotalumisSimulator::printTable() {
    wcout << "=== BEGIN CSV ===\n\"Name\";\"Count\";\"Duration (us)\"\n";
    for (auto i : table)
        wcout << "\"" << i.first.c_str() << "\";" << i.second.first << ";" << i.second.second << endl;
    wcout << "=== END CSV ===" << endl;
}

long long RotalumisSimulator::getProfilerTime() {
    return sc::duration_cast<sc::microseconds>(sc::system_clock::now().time_since_epoch()).count();
}
#endif

int RotalumisSimulator::rid() {
    return requestId++;
}