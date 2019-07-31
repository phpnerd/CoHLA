/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

package nl.ru.sws.cohla.generator.openrti.config

import nl.ru.sws.cohla.coHLA.AccuracyFault
import nl.ru.sws.cohla.coHLA.Fault
import nl.ru.sws.cohla.coHLA.FaultScenario
import nl.ru.sws.cohla.coHLA.Range
import nl.ru.sws.cohla.coHLA.TimedAbsoluteFault
import nl.ru.sws.cohla.coHLA.TimedConnectionFault
import nl.ru.sws.cohla.coHLA.TimedOffsetFault

import static nl.ru.sws.cohla.generator.openrti.Utils.*

class FaultScenarioGenerator {
  
  static def generate(FaultScenario scenario) '''
faults:«scenario.faults.size»
«FOR fault : scenario.faults»
«generate(fault)»
«ENDFOR»
'''
  
  static def generate(Fault fault) {
    if (fault instanceof TimedAbsoluteFault)
      return generate(fault as TimedAbsoluteFault)
    if (fault instanceof TimedConnectionFault)
      return generate(fault as TimedConnectionFault)
    if (fault instanceof TimedOffsetFault)
      return generate(fault as TimedOffsetFault)
    if (fault instanceof AccuracyFault)
      return generate(fault as AccuracyFault)
  }
  
  static def generate(TimedAbsoluteFault fault) '''TIMEDABS:«fault.attribute.instance.name.toFirstUpper».«fault.attribute.attribute.name»:«toVarType(fault.attribute.attribute.dataType).toString.toUpperCase»:«generate(fault.range)»:«fault.value»'''
  
  static def generate(TimedConnectionFault fault) '''TIMEDCON:«fault.attribute.instance.name.toFirstUpper».«fault.attribute.attribute.name»:«toVarType(fault.attribute.attribute.dataType).toString.toUpperCase»:«generate(fault.range)»'''
  
  static def generate(TimedOffsetFault fault) '''TIMEDOFF:«fault.attribute.instance.name.toFirstUpper».«fault.attribute.attribute.name»:«toVarType(fault.attribute.attribute.dataType).toString.toUpperCase»:«generate(fault.range)»:«fault.value»'''
  
  static def generate(AccuracyFault fault) '''VARIANCE:«fault.attribute.instance.name.toFirstUpper».«fault.attribute.attribute.name»:«toVarType(fault.attribute.attribute.dataType).toString.toUpperCase»:«fault.value»'''
  
  static def generate(Range range) {
    if (range.time !== null)
      '''«range.time»:«range.time»'''
    else {
      '''«range.startTime»:«range.endTime ?: "-"»'''
    }
  }
  
}