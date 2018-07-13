/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include "federate.h"

federate::federate() {
    rti = factory.createRTIambassador();
    fed = new federateAmbassador(this);
    scenario = nullptr;
}

federate::~federate() {
    rti->deleteObjectInstance(objectInstanceHandle, toVariableLengthData(""));
    wcout << "Deleted object instance for " << federateName.c_str() << endl;
    rti->resignFederationExecution(NO_ACTION);
    wcout << "Resigned from federation execution " << federationName.c_str() << endl;
    try {
        rti->destroyFederationExecution(toWString(federationName));
        wcout << "Destroyed federation execution " << federationName.c_str() << endl;
    } catch (FederationExecutionDoesNotExist &) {
        wcout << "Federation execution not destroyed, because it does not exist" << endl;
    } catch (FederatesCurrentlyJoined &) {
        wcout << "Federation execution not destroyed, because there are still joined federates" << endl;
    }

    delete scenario;
    delete this->fed;
    wcout << "Deleted federate " << federateName.c_str() << endl;
}

void federate::init(bool isTimeConstrained) {
    this->isConstrained = isTimeConstrained;
    init();
}

void federate::init(bool isTimeConstrained, double lookahead) {
    this->isConstrained = isTimeConstrained;
    this->isRegulating = true;
    fed->federateLookahead = lookahead;
    init();
}

void federate::init() {
    connect();
    initialiseHandles();
    publish();
    registerObjectInstance();
    setTimePolicy();
    waitAndSynchronise();
    setSubscribedAttributes();
    subscribe();
}

void federate::connect() {
    rti->connect(*fed, rti1516e::HLA_EVOKED, toWString(rtiAddress));

    try {
        rti->createFederationExecution(toWString(federationName), toWString(fomPath), L"HLAfloat64Time");
        wcout << "Created federation " << federationName.c_str() << endl;
    } catch (FederationExecutionAlreadyExists &) {
        wcout << "Federation " << federationName.c_str() << " already exists." << endl;
    }

    rti->joinFederationExecution(toWString(federateName), toWString(federationName));
    wcout << "Joined federation " << federationName.c_str() << " as " << federateName.c_str() << endl;
}

void federate::stop() {
    isStopped = true;
}

string federate::getFederateName() {
    return federateName;
}

wstring federate::getWFederateName() {
    return toWString(federateName);
}

void federate::setConfig(string federateName, string fddFile, string rtiAddress, string fileTopology, string fileScenario, string fileFaultScenario) {
    this->federateName = federateName;
    this->fomPath = fddFile;
    this->rtiAddress = "rti://" + rtiAddress + "/";

    topology = Topology(fileTopology);
    this->federationName = topology.getName();
    if (!fileScenario.empty() && fileScenario != "nil") {
        scenario = new Scenario(federateName, fileScenario, this);
        hasStopTime = scenario->hasAutoStop();
        stopTime = scenario->getAutoStopTime();
    }
    if (!fileFaultScenario.empty() && fileFaultScenario != "nil") {
        faults = Fault::parse(fileFaultScenario);
        internalFaults = filter<Fault*>(faults, [&](Fault* f) {
            bool equalInstance = f->getReference().getInstance() == federateName;
            bool noExternalConnections = !exists<Connection>(topology.getConnections(), [=](Connection c) {
                return c.getSource() == f->getReference() && !topology.isFederateWatcher(c.getTarget().getInstance());
            });
            return equalInstance && noExternalConnections;
        });
    }

    wcout << "===== Configuration =====" << endl;
    wcout << "Federation    : " << this->federationName.c_str() << endl;
    wcout << "Federate      : " << this->federateName.c_str() << endl;
    wcout << "FOM path      : " << this->fomPath.c_str() << endl;
    wcout << "RTI Address   : " << this->rtiAddress.c_str() << endl;
    wcout << "Scenario      : " << fileScenario.c_str() << endl;
    wcout << "FaultScenario : " << fileFaultScenario.c_str() << endl;
}

void federate::publish() {
    AttributeHandleSet attributes;
    for (auto &attr_pair : publishedAttributes)
        attributes.insert(attributeHandles[topology.getClass(federateName)][attr_pair.first]);
    rti->publishObjectClassAttributes(objectClassHandle, attributes);
    wcout << "Published " << publishedAttributes.size() << " attribute" << (publishedAttributes.size() != 1 ? "s" : "") << endl;
}

void federate::setSubscribedAttributes() {
    for (Connection connection : topology.getConnections())
        if (connection.getTarget().getInstance() == federateName) {
            string className = topology.getClass(connection.getSource().getInstance());
            if (objectClassHandles.count(className) != 1) {
                wcerr << "Cannot find handle for object class " << connection.getSource().getInstance().c_str() << endl;
                continue;
            }
            ObjectClassHandle oHandle = objectClassHandles[className];
            if (attributeHandles.count(className) != 1) {
                wcerr << "Cannot find attribute handle list for object class "
                      << connection.getSource().getInstance().c_str() << endl;
                continue;
            }
            if (attributeHandles[className].count(connection.getSource().getAttribute()) != 1) {
                wcerr << "Cannot find attribute handle for " << connection.getSource().getRefName().c_str() << endl;
                continue;
            }
            AttributeHandle aHandle = attributeHandles[className][connection.getSource().getAttribute()];
            subscribedAttributes.emplace_back(
                    SubscribedAttribute(connection.getSource(), oHandle, aHandle, connection.getTarget().getAttribute(),
                                        connection.getTypeName(), faults));
        }
}

void federate::subscribe() {
    map<ObjectClassHandle, AttributeHandleSet> subscribeSets;
    int i = 0;
    for (auto &subAttr : subscribedAttributes) {
        subscribeSets[subAttr.getSourceObject()].insert(subAttr.getSourceAttribute());
        i++;
    }
    for (const auto &subSet : subscribeSets)
        rti->subscribeObjectClassAttributes(subSet.first, subSet.second);
    wcout << "Subscribed to " << i << " attribute" << (i > 1 ? "s" : "") << endl;
}

void federate::waitAndSynchronise() {
    rti->registerFederationSynchronizationPoint(READY_TO_RUN, toVariableLengthData("") );
    wcout << "SynchronizationPoint " << READY_TO_RUN << " registered" << endl;
    while (!fed->hasSynchronisationPoint(READY_TO_RUN))
        rti->evokeCallback(1.0);
    waitForUser();
    rti->synchronizationPointAchieved(READY_TO_RUN);
    wcout << "SynchronisationPoint achieved. Now waiting for federation..." << endl;
    while (!fed->isSynchronised(READY_TO_RUN))
        rti->evokeCallback(1.0);
    wcout << "Federation is ready to run" << endl;
}

void federate::setTimePolicy() {
    if (this->isRegulating) {
        rti->enableTimeRegulation(HLAfloat64Interval(fed->federateLookahead));
        while (!fed->isRegulating)
            rti->evokeCallback(0.05);
        wcout << "Federate is now time regulating" << endl;
    }
    if (this->isConstrained) {
        rti->enableTimeConstrained();
        while (!fed->isConstrained)
            rti->evokeCallback(0.05);
        wcout << "Federate is now time constrained" << endl;
    }
    wcout << "Time policy set" << endl;
}

void federate::registerObjectInstance() {
    rti->reserveObjectInstanceName(toWString(federateName));
    while(!fed->isInstanceNameReserved)
        rti->evokeCallback(1.0);
    this->objectInstanceHandle = rti->registerObjectInstance(this->objectClassHandle, toWString(federateName));
    wcout << "Registered object instance" << endl;
}

void federate::discoverObjectInstanceHandle(ObjectInstanceHandle oiHandle, ObjectClassHandle ocHandle, wstring name) {
    for (auto &fedInstance : topology.getFederates()) {
        if (ocHandle == objectClassHandles[fedInstance.getClass()] && name == toWString(fedInstance.getName())) {
            objectInstanceHandles[fedInstance.getName()] = oiHandle;
            break;
        }
    }
    for (auto &subAttr : subscribedAttributes) {
        if (toWString(subAttr.getSourceReference().getInstance()) == name)
            subAttr.setSourceInstance(oiHandle);
    }
}

void federate::provideAttributeValueUpdate(ObjectInstanceHandle oiHandle, AttributeHandleSet attributes) {
    bool b = publishOnlyChanges;
    publishOnlyChanges = false;
    if (oiHandle == objectInstanceHandle)
        publishAttributeValues(getLbts());
    publishOnlyChanges = b;
}

void federate::publishAttributeValues() {
    publishAttributeValues(-1);
}

void federate::publishAttributeValues(long double time) {
    AttributeHandleValueMap attributes;
    for (auto &attr_pair : publishedAttributes)
        if (!publishOnlyChanges || (publishOnlyChanges && attr_pair.second.hasChanged()))
            attributes[attributeHandles[topology.getClass(federateName)][attr_pair.first]] = attributeValues[attr_pair.first];
    if (attributes.empty())
        return;
    if (fed->isRegulating && time > 0)
        rti->updateAttributeValues(objectInstanceHandle, attributes, toVariableLengthData(""), convertTime(time));
    else
        rti->updateAttributeValues(objectInstanceHandle, attributes, toVariableLengthData(""));
}

void federate::reflectAttributeValues(ObjectInstanceHandle oiHandle, AttributeHandleValueMap attributes, long double time) {
    for (auto &subAttr : subscribedAttributes)
        if (oiHandle == subAttr.getSourceInstance() && attributes.count(subAttr.getSourceAttribute()) == 1)
            subAttr.setData(time, attributes[subAttr.getSourceAttribute()]);
}

void federate::applyModifiers(long double time) {
    if (scenario)
        scenario->apply(time);
    for (auto &fault : internalFaults)
        if (fault->doApply(time))
            setAttribute(fault->getReference().getAttribute(), fault->apply(getAttribute(fault->getReference().getAttribute())));
}

void federate::sendInteraction() {
    
}

void federate::advanceTime(double timeStep) {
    fed->isAdvancing = true;
    rti->timeAdvanceRequest(convertTime(fed->federateTime + timeStep));
    long double nextTime = awaitTimeAdvanceGrant();
    updateAttributeValues();
    applyModifiers(nextTime);
    timeAdvanceGrantListener(nextTime);
}

void federate::advanceNextMessage(double timeStep) {
    fed->isAdvancing = true;
    rti->nextMessageRequest(convertTime(fed->federateTime + timeStep));
    long double nextTime = awaitTimeAdvanceGrant();
    updateAttributeValues();
    applyModifiers(nextTime);
    timeAdvanceGrantListener(nextTime);
}

long double federate::awaitTimeAdvanceGrant() {
    while (fed->isAdvancing)
        rti->evokeCallback(1e-4);
    return fed->federateTime;
}

void federate::setPublishedChangedState(set<string> updatedAttributes) {
    for (string a : updatedAttributes)
        if (publishedAttributes.find(a) != publishedAttributes.end())
            publishedAttributes[a].update();
}

long double federate::getLbts() {
    return fed->federateTime + fed->federateLookahead;
}