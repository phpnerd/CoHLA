/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

package nl.ru.sws.cohla.generator.openrti.config

import nl.ru.sws.cohla.coHLA.Action
import nl.ru.sws.cohla.coHLA.ActionAssign
import nl.ru.sws.cohla.coHLA.ActionSocket
import nl.ru.sws.cohla.coHLA.Event
import nl.ru.sws.cohla.coHLA.Scenario
import nl.ru.sws.cohla.coHLA.ScenarioSocket

import static nl.ru.sws.cohla.generator.HLAUtils.*

class ScenarioGenerator {
  
  static def generate(Scenario s) '''
autostop:«IF s.settings !== null && !s.settings.noAutoStop»«s.settings.autoStopTime»«ELSE»0.0«ENDIF»
«val hasSockets = s.settings !== null && s.settings.sockets !== null && !s.settings.sockets.empty»
«IF hasSockets»
sockets:«s.settings.sockets.size»
«FOR sock : s.settings.sockets»
«generate(sock)»
«ENDFOR»
«ELSE»
sockets:0
«ENDIF»
events:«s.events.size»
«FOR e : s.events»
«generate(e)»
«ENDFOR»
'''
  
  static def generate(Event e) 
    '''«toString(e.time, 0.0)»:«generate(e.action)»'''
  
  static def generate(Action a) {
    if (a instanceof ActionAssign)
      '''«generate(a)»'''
    else if (a instanceof ActionSocket)
      '''«generate(a)»'''
  }
  
  static def generate(ActionAssign a)
    '''ASSIGN:«a.attribute.instance.name.toFirstUpper».«a.attribute.attribute.name»:«a.value»'''
  
  static def generate(ActionSocket a)
    '''SOCKET:«a.socket.name»:«a.bytes.map[hb | hb.bytes].flatten.join»'''
  
  static def generate(ScenarioSocket s)
    '''«s.instance.name.toFirstUpper».«s.name»:«s.host»:«s.port»'''
  
}