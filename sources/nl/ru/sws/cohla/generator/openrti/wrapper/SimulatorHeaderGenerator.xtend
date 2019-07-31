/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

package nl.ru.sws.cohla.generator.openrti.wrapper

import nl.ru.sws.cohla.coHLA.FederateObject
import nl.ru.sws.cohla.coHLA.POOSLConfig
import nl.ru.sws.cohla.coHLA.FederateType

import static nl.ru.sws.cohla.generator.openrti.Utils.*

class SimulatorHeaderGenerator {
  
  static def generate(FederateObject f) '''
«val uName = f.name.toFirstUpper»
#ifndef «f.name.toUpperCase»SIMULATOR_H
#define «f.name.toUpperCase»SIMULATOR_H

#include <map>
«switch(f.type) {
case FMU: '''
#include <fmuWrapper.h>

class «uName»Simulator : public FmuWrapper {'''
case POOSL: '''
#include <rotsim.h>

class «uName»Simulator : public RotalumisSimulator {'''
case COLSIM: '''
#include <col_sim.h>

class «uName»Simulator : public CollisionSimulator {'''
default: '''
class «uName»Simulator {'''
}»

public:
  «switch (f.type) {
    case FMU: '''«uName»Simulator(string fmuFile, string tmpDir, string name);'''
    case POOSL: '''«uName»Simulator(string hostname, uint16_t port, string pooslModel);'''
    case COLSIM: '''«uName»Simulator(vector<string>& collisionModels, bool do_render);'''
    default: '''«uName»Simulator();'''
}»
    void updateAttributeValues() override;
    «FOR a : f.attributes»
    «toVarDecl(a, true)»;
    «IF !a.collision»
    void set«a.name.toFirstUpper»(«toVarDecl(a, false)»);
    «ENDIF»
    «ENDFOR»
    «IF f.type == FederateType.POOSL»
    «FOR p : f.parameters»
    void init«p.name.toFirstUpper»(«toVarType(p.dataType)»);
    «ENDFOR»
    «ENDIF»
    
protected:
    void init();

private:
    «IF f.type == FederateType.POOSL»
    «FOR p : (f.config as POOSLConfig).processes»
    string «p.name.toUpperCase»_PATH = "«p.path»";
    T_inspect_process process«p.name.toFirstUpper»;
    «ENDFOR»
    «ENDIF»
    «FOR a : f.attributes»
    «IF f.type == FederateType.POOSL»
    string «a.name.toUpperCase»_NAME = "«a.processProperty.attributeName»";
    «ELSE»
    string «a.name.toUpperCase»_NAME = "«IF a.alias === null»«a.name»«ELSE»«a.alias.alias»«ENDIF»";
    «ENDIF»
    «toVarDecl(a, false, '_')»;
    «toVarDecl(a, false)»;
    «ENDFOR»
    «IF f.type == FederateType.POOSL»
    «FOR p : f.parameters»
    string _«p.name.toUpperCase»_NAME = "«p.alias»";
    «ENDFOR»
    «ENDIF»

};


#endif //«f.name.toUpperCase»SIMULATOR_H
    '''
  
}