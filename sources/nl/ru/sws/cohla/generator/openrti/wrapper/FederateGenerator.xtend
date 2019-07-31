/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

package nl.ru.sws.cohla.generator.openrti.wrapper

import java.util.List
import nl.ru.sws.cohla.coHLA.FederateObject
import nl.ru.sws.cohla.coHLA.FederateType
import nl.ru.sws.cohla.coHLA.MultiInputOperator
import nl.ru.sws.cohla.coHLA.PrintLevel
import nl.ru.sws.cohla.coHLA.SharingType
import nl.ru.sws.cohla.coHLA.TimePolicy

import static nl.ru.sws.cohla.generator.HLAUtils.*
import static nl.ru.sws.cohla.generator.openrti.Utils.*

class FederateGenerator {
  
  static def generate(FederateObject f, List<FederateObject> federateObjects, boolean hasMetrics, PrintLevel printLevel, boolean publishOnlyChanges) '''
«val uName = f.name.toFirstUpper»
«val hasSim = f.type == FederateType.FMU || f.type == FederateType.POOSL || f.type == FederateType.COLSIM»
«val isCsv = f.type == FederateType.CSV»
«val isColSim = f.type == FederateType.COLSIM»
«val isPoosl = f.type == FederateType.POOSL»
«val superClass = { if (isCsv) "CsvExport" else "federate" }»
«val publishedAttributes = f.attributes.filter[attr | attr.sharingType == SharingType.PUBLISH || attr.sharingType == SharingType.PUBLISH_SUBSCRIBE]»
«val isRegulating = f.timePolicy == TimePolicy.REGULATED || f.timePolicy == TimePolicy.BOTH»
«val forwardCalculation = f.type == FederateType.FMU || f.type == FederateType.NONE»
#include "«uName»Federate.h"

«IF f.type == FederateType.POOSL»
«uName»Federate::«uName»Federate(string pooslModel, string hostname, int port) :
        sim(new «uName»Simulator(move(hostname), (uint16_t)port, pooslModel)),
«ELSEIF f.type == FederateType.FMU»
«uName»Federate::«uName»Federate(string fmuModel, string tmpDir, string name, double stepSize) :
        stepSize(stepSize),
        sim(new «uName»Simulator(fmuModel, tmpDir, name)),
«ELSEIF isColSim»
«uName»Federate::«uName»Federate(vector<string>& collisionModels, bool do_render) :
        sim(new «uName»Simulator(collisionModels, do_render)),
«ELSEIF isCsv»
«uName»Federate::«uName»Federate(string csvFile, double measureTime) :
«ELSE»
«uName»Federate::«uName»Federate(double stepSize) :
        stepSize(stepSize),
«ENDIF»
        «superClass»::«superClass»(«IF isCsv»csvFile, measureTime«ENDIF») {}
    
void «uName»Federate::init() {
    «IF f.type == FederateType.FMU»
    try {
      sim->finishModelInit();
    } catch (string& errorMessage) {
      wcerr << "Error: " << errorMessage.c_str() << endl;
    }
    «ENDIF»
    publishOnlyChanges = «publishOnlyChanges.toString»;
    federate::init();
}

«IF (f.type == FederateType.FMU || f.type == FederateType.POOSL) && f.parameters !== null && !f.parameters.empty»
void «uName»Federate::initParameters(«printCounter(0, f.parameters.length, ", ", [argNr | "string param" + argNr])») {
  try {
    «var paramCounter = 0»
    «FOR param : f.parameters»
    if (param«paramCounter» != "nil")
        «switch(f.type) {
          case FMU: '''sim->«toFMUSimSetter(param.dataType)»("«param.alias»", «stringToType("param" + paramCounter++, param.dataType)»);'''
          case POOSL: '''sim->init«param.name.toFirstUpper»(«stringToType("param" + paramCounter++, param.dataType)»);'''
          default: ''''''
        }»
    «ENDFOR»
  } catch (string& errorMessage) {
      wcerr << "Error: " << errorMessage.c_str() << endl;
  }
}
«ENDIF»

void «uName»Federate::publish() {
    «FOR attr : f.attributes.filter[a | a.sharingType == SharingType.PUBLISH || a.sharingType == SharingType.PUBLISH_SUBSCRIBE]»
    publishedAttributes["«attr.name»"] = PublishedAttribute("«attr.name»");
    «ENDFOR»
    federate::publish();
}

void «uName»Federate::timeAdvanceGrantListener(long double time) {
    «IF hasSim»
    sim->advanceTime(time«IF forwardCalculation» + fed->federateLookahead«ENDIF»);
    sim->updateAttributeValues();
    setPublishedChangedState(sim->getChangedAttributes());
    «ELSEIF f.type == FederateType.NONE»
    setPublishedChangedState(changedAttributes);
    changedAttributes.clear();
    «ENDIF»
    «IF !isCsv»
    publishAttributeValues(«IF isRegulating»getLbts()«ENDIF»);
    «ELSE»
    if (time >= measureTime)
        stopLogging();
    «ENDIF»
    «IF hasSim && printLevel != PrintLevel.NONE»
    «switch printLevel {
      case STATE: '''wcout << time«IF forwardCalculation» + fed->federateLookahead«ENDIF» << ": " << federateName.c_str() << " {«FOR a : f.attributes»«IF a != f.attributes.head»,«ENDIF» «a.name»: " << sim->get«a.name.toFirstUpper»() << "«ENDFOR» }" << endl;'''
      case TIME: '''wcout << "\rTime: " << time«IF forwardCalculation» + fed->federateLookahead«ENDIF»;'''
      default: ''''''
    }»
    «ELSEIF f.type == FederateType.NONE && printLevel != PrintLevel.NONE»
    «switch printLevel {
      case STATE: '''wcout << time«IF forwardCalculation» + fed->federateLookahead«ENDIF» << ": " << federateName.c_str() << " {«FOR a : f.attributes»«IF a != f.attributes.head»,«ENDIF» «a.name»: " << «a.name.toFirstLower» << "«ENDFOR» }" << endl;'''
      case TIME: '''wcout << "\tTime: " << time«IF f.type == FederateType.FMU» + fed->federateLookahead«ENDIF»;'''
      default: ''''''
    }»
    «ENDIF»
}

void «uName»Federate::receiveInteraction(InteractionClassHandle icHandle, ParameterHandleValueMap attributes, long double time) {

}

void «uName»Federate::run() {
    «IF !isColSim && !isCsv»
    «IF !isPoosl»
    timeAdvanceGrantListener(fed->federateTime); // Push initial values
    «ENDIF»
    «IF forwardCalculation»«toDefaultStep(f, true)»;«ENDIF»
    while(!isStopped && (!hasStopTime || (hasStopTime && stopTime > fed->federateTime)))
      «toDefaultStep(f, false)»;
    «ENDIF»
    «IF isCsv || isColSim»
    while(!isStopped«IF isCsv» && measureTime > fed->federateTime«ENDIF»)
      advanceNextMessage(«IF isCsv»measureTime«ELSEIF isColSim»numeric_limits<double>::max()«ENDIF»);
    «ENDIF»
}

void «uName»Federate::initialiseHandles() {
    «FOR obj : federateObjects»
    objectClassHandles["«obj.name»"] = rti->getObjectClassHandle(L"HLAobjectRoot.«obj.name»");
    «IF obj.name.equals(f.name)»
    objectClassHandle = objectClassHandles["«obj.name»"];
    «ENDIF» 
    «FOR attr : obj.attributes»
    attributeHandles["«obj.name»"]["«attr.name»"] = rti->getAttributeHandle(objectClassHandles["«obj.name»"], L"«attr.name»");
    «ENDFOR»
    «ENDFOR»
    «IF hasMetrics»objectClassHandles["MetricProcessor"] = rti->getObjectClassHandle(L"HLAobjectRoot.MetricProcessor");«ENDIF»
    wcout << "Initialised handles" << endl;
}

void «uName»Federate::publishAttributeValues(«IF isRegulating»long double time«ENDIF») {
    AttributeHandleValueMap attributes;
    «IF hasSim»
    «FOR a : publishedAttributes»
    attributeValues["«a.name»"] = «toHla(a.dataType)»(sim->get«a.name.toFirstUpper»()).encode();
    «ENDFOR»
    «ELSEIF f.type == FederateType.NONE»
    «FOR a : publishedAttributes»
    attributeValues["«a.name»"] = «toHla(a.dataType)»(«a.name.toFirstLower»).encode();
    «ENDFOR»
    «ENDIF»
    federate::publishAttributeValues(«IF isRegulating»time«ENDIF»);
}

«IF !isCsv»
void «uName»Federate::updateAttributeValues() {
    «IF hasSim || f.type == FederateType.NONE»«generateAttributeUpdateValues(f, federateObjects, f.type)»«ENDIF»
}

void «uName»Federate::setAttribute(string attribute, string value) {
    «FOR attr : f.attributes»
        «IF hasSim && !attr.collision»
        if (attribute == "«attr.name»")
            sim->set«attr.name.toFirstUpper»(fromString<«toVarType(attr.dataType)»>(value));
        «ELSEIF f.type == FederateType.NONE»
        if (attribute == "«attr.name»") {
            changedAttributes.emplace("«attr.name»");
            «attr.name.toFirstLower» = fromString<«toVarType(attr.dataType)»>(value);
        }
        «ENDIF»
    «ENDFOR»
}

void «uName»Federate::setAttribute(string attribute, VariableLengthData data) {
    «FOR attr : f.attributes»
        «IF hasSim && !attr.collision»
        if (attribute == "«attr.name»")
            sim->set«attr.name.toFirstUpper»(toType<«toVarType(attr.dataType)»>(data));
        «ELSEIF f.type == FederateType.NONE»
        if (attribute == "«attr.name»") {
            changedAttributes.emplace("«attr.name»");
            «attr.name.toFirstLower» = toType<«toVarType(attr.dataType)»>(data);
        }
        «ENDIF»
    «ENDFOR»
}

VariableLengthData «uName»Federate::getAttribute(string attribute) {
    «FOR attr : f.attributes»
        «IF hasSim && !attr.collision»
        if (attribute == "«attr.name»")
            return toVLD(sim->get«attr.name.toFirstUpper»());
        «ELSEIF f.type == FederateType.NONE»
        if (attribute == "«attr.name»")
            return toVLD(«attr.name.toFirstLower»);
        «ENDIF»
    «ENDFOR»
}
«ENDIF»
    '''
    
    static def generateAttributeUpdateValues(FederateObject f, List<FederateObject> federateObjects, FederateType simType) '''
        «val subscribedAttributes = getSubscribedAttributes(f, federateObjects)»
        «FOR attribute : subscribedAttributes»
pair<bool, «toVarType(attribute.dataType)»> _«attribute.name.toFirstLower»;
«ENDFOR»
for (auto &attribute : subscribedAttributes) {
    if (!attribute.isRead()) {
        «FOR attribute : subscribedAttributes»
        if (attribute.getTarget() == "«attribute.name»") {
            «IF attribute.multiInputOperator == MultiInputOperator.NONE»
            _«attribute.name.toFirstLower».second = toType<«toVarType(attribute.dataType)»>(attribute.getData());
            «ELSE»
            «attribute.name.toFirstLower»Cache[attribute.getSourceReference().getRefName()] = toType<«toVarType(attribute.dataType)»>(attribute.getData());
            «ENDIF»
            _«attribute.name.toFirstLower».first = true;
        }
        «ENDFOR»
    }
}

«FOR attribute : subscribedAttributes»
    «val aType = toVarType(attribute.dataType)»
if (_«attribute.name.toFirstLower».first) {
    «IF attribute.multiInputOperator != MultiInputOperator.NONE»
        _«attribute.name.toFirstLower».second = mapValues<string, «aType»>(«attribute.name.toFirstLower»Cache, [&](«aType» x, «aType» y) {
            return x «toOperator(attribute.multiInputOperator)» y;
        });
    «ENDIF»
«switch (simType) {
  case NONE: '''«attribute.name.toFirstLower» = _«attribute.name.toFirstLower».second;'''
  default: '''    sim->set«attribute.name.toFirstUpper»(_«attribute.name.toFirstLower».second);'''
}»
}
«ENDFOR»
'''
    
    static def getSubscribedAttributes(FederateObject f, List<FederateObject> federateObjects) {
      if (f.type != FederateType.CSV)
        return f.attributes
          .filter[attr | attr.sharingType == SharingType.SUBSCRIBE || attr.sharingType == SharingType.PUBLISH_SUBSCRIBE]
          .toList
      else
        return federateObjects
          .map[o | o.attributes]
          .flatten
          .filter[a | a.sharingType == SharingType.PUBLISH || a.sharingType == SharingType.PUBLISH_SUBSCRIBE]
          .toList
    }
  
}