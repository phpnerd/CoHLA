/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef FEDAMB_H
#define FEDAMB_H

#include <RTI/NullFederateAmbassador.h>
#include <RTI/time/HLAfloat64Time.h>
#include <functional>
#include <federate.h>

using namespace rti1516e;
using namespace std;

class federate;

class federateAmbassador : public NullFederateAmbassador
{
public:
    // variables //
    long double federateTime;
    double federateLookahead;

    bool isRegulating;
    bool isConstrained;
    bool isAdvancing;
    bool isAnnounced;

    bool isInstanceNameReserved;

    // methods //
    //! Constructor
    federateAmbassador(federate* fed);
    //! Destructor
    virtual ~federateAmbassador() throw();

//    Synchronisation methods

    virtual void synchronizationPointRegistrationSucceeded(std::wstring const &label) throw(FederateInternalError);
    virtual void synchronizationPointRegistrationFailed(std::wstring const &label,  SynchronizationPointFailureReason reason) throw(FederateInternalError);
    virtual void announceSynchronizationPoint(std::wstring const &label, VariableLengthData const &theUserSuppliedTag) throw(FederateInternalError);
    virtual void federationSynchronized(std::wstring const &label, FederateHandleSet const& failedToSyncSet) throw(FederateInternalError);
    virtual bool hasSynchronisationPoint(std::wstring const &label);
    virtual bool isSynchronised(std::wstring const &label);

//    Time methods

    virtual void timeRegulationEnabled(LogicalTime const &theFederateTime) throw (FederateInternalError);
    virtual void timeConstrainedEnabled(LogicalTime const &theFederateTime) throw (FederateInternalError);
    virtual void timeAdvanceGrant(LogicalTime const &theTime) throw (FederateInternalError);

//    Object methods
    virtual void objectInstanceNameReservationSucceeded ( std::wstring const & theObjectInstanceName) throw (FederateInternalError);
    virtual void objectInstanceNameReservationFailed ( std::wstring const & theObjectInstanceName) throw (FederateInternalError);

    virtual void discoverObjectInstance ( ObjectInstanceHandle theObject, ObjectClassHandle theObjectClass, std::wstring const & theObjectInstanceName) throw (FederateInternalError);
    virtual void provideAttributeValueUpdate( ObjectInstanceHandle theObject, AttributeHandleSet const & theAttributes, VariableLengthData const & theUserSuppliedTag) throw (FederateInternalError);
    virtual void reflectAttributeValues( ObjectInstanceHandle theObject, AttributeHandleValueMap const & theAttributeValues, VariableLengthData const & theUserSuppliedTag, OrderType sentOrder, TransportationType theType, SupplementalReflectInfo theReflectInfo) throw (FederateInternalError);
    virtual void reflectAttributeValues( ObjectInstanceHandle theObject, AttributeHandleValueMap const & theAttributeValues, VariableLengthData const & theUserSuppliedTag, OrderType sentOrder, TransportationType theType, LogicalTime const & theTime, OrderType receivedOrder, SupplementalReflectInfo theReflectInfo) throw (FederateInternalError);
    virtual void reflectAttributeValues( ObjectInstanceHandle theObject, AttributeHandleValueMap const & theAttributeValues, VariableLengthData const & theUserSuppliedTag, OrderType sentOrder, TransportationType theType, LogicalTime const & theTime, OrderType receivedOrder, MessageRetractionHandle theHandle, SupplementalReflectInfo theReflectInfo) throw (FederateInternalError);

    virtual void receiveInteraction( InteractionClassHandle theInteraction, ParameterHandleValueMap const & theParameterValues, VariableLengthData const & theUserSuppliedTag, OrderType sentOrder, TransportationType theType, SupplementalReceiveInfo theReceiveInfo) throw (FederateInternalError);
    virtual void receiveInteraction( InteractionClassHandle theInteraction, ParameterHandleValueMap const & theParameterValues, VariableLengthData const & theUserSuppliedTag, OrderType sentOrder, TransportationType theType, LogicalTime const & theTime, OrderType receivedOrder, SupplementalReceiveInfo theReceiveInfo) throw (FederateInternalError);
    virtual void receiveInteraction( InteractionClassHandle theInteraction, ParameterHandleValueMap const & theParameterValues, VariableLengthData const & theUserSuppliedTag, OrderType sentOrder, TransportationType theType, LogicalTime const & theTime, OrderType receivedOrder, MessageRetractionHandle theHandle, SupplementalReceiveInfo theReceiveInfo) throw (FederateInternalError);

private:
    map<wstring, bool> synchronisationPoints;
    federate* fed;

    static HLAfloat64Time convertTime( LogicalTime const &theTime ) {
        HLAfloat64Time castedTime = (HLAfloat64Time)theTime;
        return castedTime;
    }

    static std::wstring variableLengthDataToWstring(const rti1516e::VariableLengthData& variableLengthData) {
        if (!variableLengthData.size())
            return std::wstring();
        return std::wstring((const wchar_t*) variableLengthData.data(),
                            variableLengthData.size() / sizeof(std::wstring::value_type));
    }

};

#endif // FEDAMB_H