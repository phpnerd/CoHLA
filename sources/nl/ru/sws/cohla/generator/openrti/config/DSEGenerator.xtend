/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

package nl.ru.sws.cohla.generator.openrti.config

import nl.ru.sws.cohla.coHLA.DSEAttrInit
import nl.ru.sws.cohla.coHLA.DSEConfig
import nl.ru.sws.cohla.coHLA.DSEFederateConfig
import nl.ru.sws.cohla.coHLA.DSESituationConfig

class DSEGenerator {
  
  static def generate(DSEConfig dse) '''
mode:«dse.sweepMode»
«IF dse.scenario !== null»scenario:«dse.scenario.name»«ENDIF»
«IF dse.faultScenario !== null»faultScenario:«dse.faultScenario.name»«ENDIF»
«IF dse.situations !== null && !dse.situations.empty»«dse.situations.map[generate].join»«ENDIF»
«IF dse.configurations !== null && !dse.configurations.empty»«dse.configurations.map[generate].join»«ENDIF»
«IF dse.attributes !== null && !dse.attributes.empty»«dse.attributes.map[generate].join»«ENDIF»'''

  static def generate(DSEAttrInit attr) '''
INIT:«attr.attr.instance.name.toFirstUpper».«attr.attr.attribute.name.toFirstLower»:[«attr.value.value»«attr.value.values.join(",")»]
  '''
  
  static def generate(DSESituationConfig sit) '''
SITU:[«sit.situations.map[s | "\"" + s.name + "\""].join(",")»]
  '''
  
  static def generate(DSEFederateConfig config) '''
CONF:«config.instance.name.toFirstUpper»:[«config.configurations.map[c | "\"" + c.name + "\""].join(",")»]
  '''
  
}