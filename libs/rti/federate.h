/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef FED_H
#define FED_H

#include <memory>
#include <RTI/RTIambassador.h>
#include <RTI/RTIambassadorFactory.h>
#include <RTI/LogicalTime.h>
#include <RTI/time/HLAfloat64Time.h>
#include <RTI/encoding/BasicDataElements.h>
#include <RTI/LogicalTimeFactory.h>
#include <iostream>
#include <cstring>
#include <list>
#include <unordered_set>
#include <fstream>
#include <random>
#include <chrono>
#include "Utils.h"
#include "federateAmbassador.h"
#include "models/Topology.h"
#include "models/FederateAttributeReference.h"
#include "models/Connection.h"
#include "models/SubscribedAttribute.h"
#include "models/PublishedAttribute.h"
#include "scenario/Scenario.h"
#include "faults/Fault.h"
#include "socket.h"

#define READY_TO_RUN L"ReadyToRun"

using namespace rti1516e;
using namespace std;

class federateAmbassador;

static bool volatile isStopped = false;

class federate {

public:
    federate();
    virtual ~federate();
    virtual void init();
    void init(bool isTimeConstrained);
    void init(bool isTimeConstrained, double lookahead);
    virtual void setConfig(string, string, string, string, string, string);
    virtual void stop();
    string getFederateName();
    wstring getWFederateName();
    void provideAttributeValueUpdate(ObjectInstanceHandle oiHandle, AttributeHandleSet attributes);
    virtual void publish();
    virtual void subscribe();
    virtual void run() = 0;
    virtual void timeAdvanceGrantListener( long double ) = 0;
    void discoverObjectInstanceHandle(ObjectInstanceHandle oiHandle, ObjectClassHandle ocHandle, wstring name);
    void reflectAttributeValues(ObjectInstanceHandle oiHandle, AttributeHandleValueMap attributes, long double time);
    virtual void receiveInteraction(InteractionClassHandle icHandle, ParameterHandleValueMap attributes, long double time) = 0;

    virtual void setAttribute(string, string) = 0;
    virtual void setAttribute(string, VariableLengthData) = 0;
    virtual VariableLengthData getAttribute(string) = 0;

protected:
    std::unique_ptr<rti1516e::RTIambassador> rti;
    federateAmbassador* fed;

    //federate configuration
    std::string federationName, federateName, fomPath, rtiAddress;
    rti1516e::RTIambassadorFactory factory;

    ObjectClassHandle objectClassHandle;
    ObjectInstanceHandle objectInstanceHandle;
    map<string, ObjectClassHandle> objectClassHandles;
    map<string, ObjectInstanceHandle> objectInstanceHandles;
    map<string, map<string, AttributeHandle>> attributeHandles;
    map<string, VariableLengthData> attributeValues;
    list<SubscribedAttribute> subscribedAttributes;
    map<string, PublishedAttribute> publishedAttributes;

    Topology topology;
    Scenario* scenario;

    bool hasStopTime = false;
    double stopTime = 0.0;

    bool publishOnlyChanges = true;

    void registerObjectInstance();

    void sendInteraction();
    void advanceTime( double );
    void advanceNextMessage( double );
    long double awaitTimeAdvanceGrant();
    long double getLbts();
    virtual void publishAttributeValues();
    virtual void publishAttributeValues(long double);

    void setPublishedChangedState(set<string>);

    static std::wstring toWString(std::string str) {
        std::wstring str2(str.length(), L' ');
        std::copy(str.begin(), str.end(), str2.begin());
        return str2;
    }

    static HLAfloat64Time convertTime(LogicalTime const &theTime) {
        HLAfloat64Time castedTime = (HLAfloat64Time)theTime;
        return castedTime;
    }

    static HLAfloat64Time convertTime(long double const &theTime) {
        return (HLAfloat64Time) theTime;
    }

    static rti1516e::VariableLengthData toVariableLengthData(const char* s) {
        rti1516e::VariableLengthData variableLengthData;
        if (s)
            variableLengthData.setData(s, strlen(s));
        return variableLengthData;
    }

    static void waitForUser() {
        wcout << " >>>>>>>>>> Press Enter to Continue <<<<<<<<<<";
        string line;
        getline(cin, line);
    }

private:
    bool isRegulating = false, isConstrained = false;

    list<Fault*> faults, internalFaults;

    void connect();
    void setSubscribedAttributes();

    void applyModifiers(long double);

    void waitAndSynchronise();
    void setTimePolicy();

    virtual void initialiseHandles() = 0;
    virtual void updateAttributeValues() = 0;

};

#endif /* CPPFEDERATE_H_ */
