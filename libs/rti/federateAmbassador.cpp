/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <iostream>
#include "federateAmbassador.h"

using namespace std;
using namespace rti1516e;

federateAmbassador::federateAmbassador(federate* fed):
    federateTime(0.0),
    federateLookahead(1.0),
    isRegulating(false),
    isConstrained(false),
    isAdvancing(false),
    isAnnounced(false),
    isInstanceNameReserved(false),
    fed(fed)
{
}

federateAmbassador::~federateAmbassador() throw() = default;

void federateAmbassador::synchronizationPointRegistrationSucceeded( std::wstring const &label ) throw(FederateInternalError) {
	wcout << L"Successfully registered sync point: " << label << endl;
}

void federateAmbassador::synchronizationPointRegistrationFailed( std::wstring const &label,  SynchronizationPointFailureReason reason ) throw(FederateInternalError) {
    wcout << L"Failed to register sync point: " << label << endl;
}

void federateAmbassador::announceSynchronizationPoint( std::wstring const &label, VariableLengthData const &theUserSuppliedTag ) throw(FederateInternalError) {
    synchronisationPoints[label] = false;
	wcout << L"Synchronization point announced: " << label << endl;
}

void federateAmbassador::federationSynchronized( std::wstring const &label, FederateHandleSet const& failedToSyncSet ) throw(FederateInternalError) {
    synchronisationPoints[label] = true;
	wcout << L"Federation synchronized: " << label << endl;
}

bool federateAmbassador::hasSynchronisationPoint( std::wstring const &label ) {
    return synchronisationPoints.count(label) > 0;
}

bool federateAmbassador::isSynchronised( std::wstring const &label ) {
    if (synchronisationPoints.count(label) > 0)
        return synchronisationPoints[label];
    else
        return false;
}

void federateAmbassador::timeRegulationEnabled(LogicalTime const &theFederateTime) throw (FederateInternalError) {
    this->isRegulating = true;
    this->federateTime = convertTime(theFederateTime);
}

void federateAmbassador::timeConstrainedEnabled(LogicalTime const &theFederateTime) throw (FederateInternalError) {
    this->isConstrained = true;
    this->federateTime = convertTime(theFederateTime);
}

void federateAmbassador::timeAdvanceGrant(LogicalTime const &theTime) throw (FederateInternalError) {
    this->federateTime = convertTime(theTime);
    this->isAdvancing = false;
}

void federateAmbassador::objectInstanceNameReservationSucceeded(std::wstring const &theObjectInstanceName) throw (FederateInternalError) {
    if (!theObjectInstanceName.compare(fed->getWFederateName()))
        isInstanceNameReserved = true;
}

void federateAmbassador::objectInstanceNameReservationFailed(std::wstring const &theObjectInstanceName) throw (FederateInternalError) {
    if (!theObjectInstanceName.compare(fed->getWFederateName()))
        isInstanceNameReserved = false;
}

void federateAmbassador::discoverObjectInstance (ObjectInstanceHandle theObject, ObjectClassHandle theObjectClass, std::wstring const & theObjectInstanceName) throw (FederateInternalError) {
    wcout << "Instance " << theObjectInstanceName << " discovered" << endl;
    fed->discoverObjectInstanceHandle(theObject, theObjectClass, theObjectInstanceName);
}

void federateAmbassador::provideAttributeValueUpdate(ObjectInstanceHandle theObject, const AttributeHandleSet &theAttributes, const VariableLengthData &theUserSuppliedTag) throw (FederateInternalError) {
    fed->provideAttributeValueUpdate(theObject, theAttributes);
}

void federateAmbassador::reflectAttributeValues( ObjectInstanceHandle theObject, AttributeHandleValueMap const & theAttributeValues, VariableLengthData const & theUserSuppliedTag, OrderType sentOrder, TransportationType theType, SupplementalReflectInfo theReflectInfo) throw (FederateInternalError) {
    fed->reflectAttributeValues(theObject, theAttributeValues, federateTime);
}

void federateAmbassador::reflectAttributeValues( ObjectInstanceHandle theObject, AttributeHandleValueMap const & theAttributeValues, VariableLengthData const & theUserSuppliedTag, OrderType sentOrder, TransportationType theType, LogicalTime const & theTime, OrderType receivedOrder, SupplementalReflectInfo theReflectInfo) throw (FederateInternalError) {
    fed->reflectAttributeValues(theObject, theAttributeValues, convertTime(theTime));
}

void federateAmbassador::reflectAttributeValues( ObjectInstanceHandle theObject, AttributeHandleValueMap const & theAttributeValues, VariableLengthData const & theUserSuppliedTag, OrderType sentOrder, TransportationType theType, LogicalTime const & theTime, OrderType receivedOrder, MessageRetractionHandle theHandle, SupplementalReflectInfo theReflectInfo) throw (FederateInternalError) {
    reflectAttributeValues(theObject, theAttributeValues, theUserSuppliedTag, sentOrder, theType, theTime, receivedOrder, theReflectInfo);
}

void federateAmbassador::receiveInteraction( InteractionClassHandle theInteraction, ParameterHandleValueMap const & theParameterValues, VariableLengthData const & theUserSuppliedTag, OrderType sentOrder, TransportationType theType, SupplementalReceiveInfo theReceiveInfo) throw (FederateInternalError) {
    fed->receiveInteraction(theInteraction, theParameterValues, federateTime);
}

void federateAmbassador::receiveInteraction( InteractionClassHandle theInteraction, ParameterHandleValueMap const & theParameterValues, VariableLengthData const & theUserSuppliedTag, OrderType sentOrder, TransportationType theType, LogicalTime const & theTime, OrderType receivedOrder, SupplementalReceiveInfo theReceiveInfo) throw (FederateInternalError) {
    fed->receiveInteraction(theInteraction, theParameterValues, convertTime(theTime));
}

void federateAmbassador::receiveInteraction( InteractionClassHandle theInteraction, ParameterHandleValueMap const & theParameterValues, VariableLengthData const & theUserSuppliedTag, OrderType sentOrder, TransportationType theType, LogicalTime const & theTime, OrderType receivedOrder, MessageRetractionHandle theHandle, SupplementalReceiveInfo theReceiveInfo) throw (FederateInternalError) {
    receiveInteraction(theInteraction, theParameterValues, theUserSuppliedTag, sentOrder, theType, theTime, receivedOrder, theReceiveInfo);
}