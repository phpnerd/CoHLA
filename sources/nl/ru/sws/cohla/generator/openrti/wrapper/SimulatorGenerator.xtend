/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

package nl.ru.sws.cohla.generator.openrti.wrapper

import nl.ru.sws.cohla.coHLA.FederateObject
import nl.ru.sws.cohla.coHLA.POOSLConfig
import nl.ru.sws.cohla.coHLA.FederateType

import static nl.ru.sws.cohla.generator.openrti.Utils.*

class SimulatorGenerator {
  
  static def generate(FederateObject f) '''
«val uName = f.name.toFirstUpper»
#include "«uName»Simulator.h"

«switch (f.type) {
case FMU: '''«uName»Simulator::«uName»Simulator(string fmuPath, string tmpDir, string name) :'''
case POOSL: '''«uName»Simulator::«uName»Simulator(string hostname, uint16_t port, string pooslModel) :'''
case COLSIM: '''«uName»Simulator::«uName»Simulator(vector<string>& collisionModels, bool do_render) :'''
default: '''«uName»Simulator::«uName»Simulator() :'''
}»
    «FOR a : f.attributes»
    «toDefaultDecl(a)»«IF !a.equals(f.attributes.last)»,«ENDIF»
    «ENDFOR»
    «switch (f.type) {
    case FMU: ''',FmuWrapper(fmuPath, tmpDir, name)'''
    case POOSL: ''',RotalumisSimulator(hostname, port, pooslModel)'''
    case COLSIM: ''',CollisionSimulator(collisionModels, do_render)'''
    default: ''''''
    }»
    
{
    this->init();
}

void «uName»Simulator::init() {
    «IF f.type == FederateType.POOSL»
    «FOR p : (f.config as POOSLConfig).processes»
    this->process«p.name.toFirstUpper» = getProcess(«p.name.toUpperCase»_PATH);
    «ENDFOR»
    «ENDIF»
    «FOR a : f.attributes»
    changedAttributes.emplace("«a.name»");
    «ENDFOR»
}

void «uName»Simulator::updateAttributeValues() {
    «IF f.type == FederateType.POOSL»
    «FOR p : (f.config as POOSLConfig).processes»
    this->process«p.name.toFirstUpper» = getProcess(«p.name.toUpperCase»_PATH);
    «ENDFOR»
    «ENDIF»
    «FOR a : f.attributes»
    _«a.name.toFirstLower» = «a.name.toFirstLower»;
    «toSimGetter(a, f.type)»
    if (_«a.name.toFirstLower» != «a.name.toFirstLower»)
        changedAttributes.emplace("«a.name»");
    «ENDFOR»
}
«FOR a : f.attributes»

«toVarDecl(a, true, uName + "Simulator::")» {
    return «a.name.toFirstLower»;
}
«IF !a.collision»
void «uName»Simulator::set«a.name.toFirstUpper»(«toVarDecl(a, false)») {
    if («a.name.toFirstLower» != this->«a.name.toFirstLower»)
        changedAttributes.emplace("«a.name»");
    this->«a.name.toFirstLower» = «a.name.toFirstLower»;
    «toSimSetter(a, f.type)»;
}

«ENDIF»
«ENDFOR»
«IF f.type == FederateType.POOSL»
«FOR p : f.parameters»

void «uName»Simulator::init«p.name.toFirstUpper»(«toVarType(p.dataType)» value) {
    setElement(process«p.process.name.toFirstUpper», «p.process.name.toUpperCase»_PATH, _«p.name.toUpperCase»_NAME, value);
}
«ENDFOR»
«ENDIF»
    '''
  
}