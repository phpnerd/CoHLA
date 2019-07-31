/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

package nl.ru.sws.cohla.generator.openrti.config

import java.util.HashMap
import java.util.List
import nl.ru.sws.cohla.coHLA.AttributeConnection
import nl.ru.sws.cohla.coHLA.Connection
import nl.ru.sws.cohla.coHLA.FederateType
import nl.ru.sws.cohla.coHLA.Federation
import nl.ru.sws.cohla.coHLA.Interface
import nl.ru.sws.cohla.coHLA.LoggerConnection
import nl.ru.sws.cohla.coHLA.ObjectConnection

import static nl.ru.sws.cohla.generator.HLAUtils.*
import static nl.ru.sws.cohla.generator.openrti.Utils.*

class TopologyGenerator {
  
  static def generate(Federation fed, List<Interface> interfacesList) '''
  «val interfaces = getInterfaceMap(interfacesList)»
name:«fed.name»
federates:«fed.instances.size»
«FOR i : fed.instances»
«i.name.toFirstUpper»:«i.federate.name.toFirstUpper»:«IF i.federate.type == FederateType.CSV»true«ELSE»false«ENDIF»
«ENDFOR»
connections:«countConnections(fed.connections, interfaces)»
«FOR c : fed.connections»
«generate(c, interfacesList)»
«ENDFOR»
'''
  static def generate(Connection c, List<Interface> interfaces) {
    if (c instanceof AttributeConnection) return generate(c)
    if (c instanceof LoggerConnection) return generate(c)
    if (c instanceof ObjectConnection) return generate(c, interfaces)
    return ''''''
  }

  static def generate(AttributeConnection c)
'''«FOR a : c.outAttributes»
«c.inAttribute.instance.name.toFirstUpper».«c.inAttribute.attribute.name»-«a.instance.name.toFirstUpper».«a.attribute.name»:«toVarType(a.attribute.dataType).toString.toUpperCase»
«ENDFOR»'''

  static def generate(LoggerConnection c)
'''«FOR a : c.outAttributes»
«c.logger.name.toFirstUpper».«a.instance.name.toFirstUpper»«a.attribute.name.toFirstUpper»-«a.instance.name.toFirstUpper».«a.attribute.name»:«toVarType(a.attribute.dataType).toString.toUpperCase»
«ENDFOR»'''

  static def generate(ObjectConnection c, List<Interface> interfacesList) {
    val interfaces = getInterfaceMap(interfacesList)
    val name1 = c.instance1.federate.name
    val name2 = c.instance2.federate.name
    var s = ""
    if (!(interfaces.containsKey(name1) && interfaces.get(name1).containsKey(name2))) {
      println("Interface to connect " + name1 + " with " + name2 + " not found, skipping!")
      return s
    }
    val iface = interfaces.get(name1).get(name2)
    for (iConnection : iface.connections) {
      val correctOrder = iConnection.in.objectClass.name == c.instance1.federate.name
      var inInstance = c.instance1
      var outInstance = c.instance2
      if (!correctOrder) {
        inInstance = c.instance2
        outInstance = c.instance1
      }
      s += '''«inInstance.name.toFirstUpper».«iConnection.in.attribute.name»-«outInstance.name.toFirstUpper».«iConnection.out.attribute.name»:«toVarType(iConnection.out.attribute.dataType).toString.toUpperCase»
'''
    }
    return s
  }

  static def countConnections(List<Connection> connections, HashMap<String, HashMap<String, Interface>> interfaces) {
    var c = 0
    for (con : connections) {
      if (con instanceof AttributeConnection) {
        c += con.outAttributes.size
      } else if (con instanceof LoggerConnection) {
        c += con.outAttributes.size
      } else if (con instanceof ObjectConnection) {
        c += interfaces.get(con.instance1.federate.name).get(con.instance2.federate.name).connections.size
      }
    }
    return c
  }
  
}