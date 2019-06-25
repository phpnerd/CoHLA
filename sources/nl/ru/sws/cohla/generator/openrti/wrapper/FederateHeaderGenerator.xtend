/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

package nl.ru.sws.cohla.generator.openrti.wrapper

import nl.ru.sws.cohla.coHLA.FederateObject
import nl.ru.sws.cohla.coHLA.FederateType
import nl.ru.sws.cohla.coHLA.MultiInputOperator
import nl.ru.sws.cohla.coHLA.TimePolicy
import nl.ru.sws.cohla.generator.openrti.Utils

class FederateHeaderGenerator {
  
  static def generate(FederateObject f) '''
«val uName = f.name.toFirstUpper»
«var hasSim = f.type == FederateType.FMU || f.type == FederateType.POOSL || f.type == FederateType.COLSIM»
«var superClass = { if (f.type == FederateType.CSV) "CsvExport" else "federate" }»
«val isRegulating = f.timePolicy == TimePolicy.REGULATED || f.timePolicy == TimePolicy.BOTH»
#ifndef «uName»FEDERATE_H
#define «uName»FEDERATE_H

#include <«superClass».h>
«IF f.type == FederateType.CSV»
#include <limits>
«ENDIF»
«IF hasSim»
#include "«uName»Simulator.h"
«ENDIF»

class «uName»Federate : public «superClass» {
public:
    «IF f.type == FederateType.POOSL»
    «uName»Federate(string pooslModel, string hostname, int port);
    «ELSEIF f.type == FederateType.FMU»
    «uName»Federate(string fmuModel, string tmpDir, string name, double stepSize);
    «ELSEIF f.type == FederateType.COLSIM»
    «uName»Federate(vector<string>& collisionModels, bool do_render);
    «ELSEIF f.type == FederateType.CSV»
    «uName»Federate(string csvFile, double measureTime = numeric_limits<double>::max());
    «ELSE»
    «uName»Federate(double);
    «ENDIF»
    void run() override;
    void init() override;
    void publish() override;
    «IF (f.type == FederateType.FMU || f.type == FederateType.POOSL) && f.parameters !== null && !f.parameters.empty»
    void initParameters(«Utils.printCounter(0, f.parameters.length, ", ", [argNr | "string param" + argNr])»);
    «ENDIF»
    void receiveInteraction(InteractionClassHandle icHandle, ParameterHandleValueMap attributes, long double time) override;

protected:

private:
    «IF hasSim»
    «uName»Simulator* sim;
    «IF f.type == FederateType.FMU»
    double stepSize;
    «ENDIF»
    «ENDIF»
    «IF f.type == FederateType.NONE»
    double stepSize;
    set<string> changedAttributes;
    «FOR a : f.attributes»
    «Utils.toVarDecl(a, false)»;
    «ENDFOR»
    «ENDIF»
    
    «FOR a : f.attributes.filter[a | a.multiInputOperator != MultiInputOperator.NONE]»
    map<string, «Utils.toVarType(a.dataType)»> «a.name.toFirstLower»Cache;
    «ENDFOR»
    
    void initialiseHandles() override;
    void publishAttributeValues(«IF isRegulating»long double«ENDIF») override;
    void timeAdvanceGrantListener(long double time) override;
    «IF f.type !== FederateType.CSV»
    void updateAttributeValues() override;
    void setAttribute(string, string) override;
    void setAttribute(string, VariableLengthData) override;
    VariableLengthData getAttribute(string) override;
    «ENDIF»
};

#endif //«uName»FEDERATE_H
    '''
  
}
