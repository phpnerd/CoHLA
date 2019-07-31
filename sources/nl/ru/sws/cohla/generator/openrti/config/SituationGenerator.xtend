/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

package nl.ru.sws.cohla.generator.openrti.config

import java.util.List
import nl.ru.sws.cohla.coHLA.ApplyModelConfiguration
import nl.ru.sws.cohla.coHLA.AttributeConfigurator
import nl.ru.sws.cohla.coHLA.AttributeInitialiser
import nl.ru.sws.cohla.coHLA.ModelConfiguration
import nl.ru.sws.cohla.coHLA.Situation
import nl.ru.sws.cohla.coHLA.SituationElement

import static nl.ru.sws.cohla.generator.openrti.Utils.*

class SituationGenerator {
  
  static def generate(Situation situation) '''
«generateSingle(situation)»
'''

  static def generateSingle(Situation situation)
  '''«IF situation.extends !== null»
«generateSingle(situation.extends)»
«ENDIF»
«generate(situation.elements)»'''
  
  static def generate(List<SituationElement> elements)
  '''«FOR a : elements»
«IF a instanceof ApplyModelConfiguration»
«generate(a as ApplyModelConfiguration)»
«ELSEIF a instanceof AttributeInitialiser»
«generate(a as AttributeInitialiser)»
«ENDIF»
«ENDFOR»'''
    
  static def generate(AttributeInitialiser initAttribute) 
  '''«initAttribute.reference.instance.name.toFirstUpper».«toAttributeName(initAttribute.reference.attribute)»:«initAttribute.value»'''
  
  static def generate(ApplyModelConfiguration applyModelConfiguration)
  '''«FOR a : applyModelConfiguration.configuration.initAttributes»«generate(a, applyModelConfiguration.instance.name)»
«ENDFOR»'''
  
  static def generate(ModelConfiguration modelConfiguration) 
  '''«modelConfiguration.initAttributes.map[i | generate(i, modelConfiguration.name.toFirstUpper)].join("\n")»'''
  
  static def generate(AttributeConfigurator initAttribute, String instanceName)
  '''«instanceName.toFirstUpper».«toAttributeName(initAttribute.reference.attribute)»:«initAttribute.value»'''
  
}