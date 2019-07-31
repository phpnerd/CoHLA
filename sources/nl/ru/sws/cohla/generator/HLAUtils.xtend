/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

package nl.ru.sws.cohla.generator

import java.util.HashMap
import java.util.List
import nl.ru.sws.cohla.coHLA.DataType
import nl.ru.sws.cohla.coHLA.FederateObject
import nl.ru.sws.cohla.coHLA.FederateType
import nl.ru.sws.cohla.coHLA.Interface
import nl.ru.sws.cohla.coHLA.OrderProperty
import nl.ru.sws.cohla.coHLA.SharingType
import nl.ru.sws.cohla.coHLA.TransportationProperty
import nl.ru.sws.cohla.coHLA.UpdateConditionProperty
import nl.ru.sws.cohla.coHLA.UpdateTypeProperty

class HLAUtils {
  
  static def toHla(DataType type) {
        switch (type) {
            case BOOL: '''HLAboolean'''
            case INTEGER: '''HLAinteger32BE'''
            case LONG: '''HLAinteger64BE'''
            case REAL: '''HLAfloat64BE'''
            case STRING: '''HLAASCIIstring'''
        }
    }
    
    static def toHla(UpdateConditionProperty updateCondition) {
      if (updateCondition === null)
        return '''On change'''
      switch (updateCondition.updateCondition) {
        case ON_CHANGE: '''On Change'''
        default: '''On change'''
      }
    }
    
    static def toHla(UpdateTypeProperty updateType) {
      if (updateType === null)
        return '''Conditional'''
      switch (updateType.updateType) {
        case CONDITIONAL: '''Conditional'''
        default: '''Conditional'''
      }
    }
    
    static def toHlaSharingType(FederateObject fed) {
      var isPublished = fed.attributes.map[a | a.sharingType].exists[s | s == SharingType.PUBLISH || s == SharingType.PUBLISH_SUBSCRIBE];
      var isSubscribed = fed.attributes.map[a | a.sharingType].exists[s | s == SharingType.SUBSCRIBE || s == SharingType.PUBLISH_SUBSCRIBE];
      if (isPublished && isSubscribed) toHlaSharingType(SharingType.PUBLISH_SUBSCRIBE)
      else if (isPublished) toHlaSharingType(SharingType.PUBLISH)
      else if (isSubscribed) toHlaSharingType(SharingType.SUBSCRIBE)
      else toHlaSharingType(SharingType.NEITHER)
    }
    
    static def toHlaSharingType(SharingType sharingType) {
      switch(sharingType) {
        case NEITHER: '''Neither'''
        case PUBLISH: '''Publish'''
        case SUBSCRIBE: '''Subscribe'''
        case PUBLISH_SUBSCRIBE: '''PublishSubscribe'''
      }
    }
    
    static def toHla(TransportationProperty transportation) {
      if (transportation === null)
        return '''HLAreliable'''
      switch (transportation.transportation) {
        case RELIABLE: '''HLAreliable'''
        case BEST_EFFORT: '''HLAbestEffort'''
        default: '''HLAreliable'''
      }
    }
    
    static def toHla(OrderProperty order) {
      if (order === null)
        return '''TimeStamp'''
      switch (order.order) {
        case RECEIVE: '''Receive'''
        case TIME_STAMP: '''TimeStamp'''
        default: '''TimeStamp'''
      }
    }
    
    static def toString(String r, double nullValue) '''«IF r === null»«Double.toString(nullValue)»«ELSE»«r»«ENDIF»'''
    
    static def toString(int i, int f) {
      var s = "";
      for (var j = i.toString.length; j < f; j++) s += "0"
      return s + i;
    }
    
    static def defaultExtension(FederateType type) {
      switch (type) {
        case POOSL: "poosl"
        case FMU: "fmu"
        case COLSIM: "stl"
        default: ""
      }
    }
    
    static def hasSimulator(FederateType type) {
      return type == FederateType.COLSIM || type == FederateType.FMU || type == FederateType.POOSL
    }
    
    static def getInterfaceMap(List<Interface> interfacesList) {
      var interfaces = new HashMap<String, HashMap<String, Interface>>();
      for (iface : interfacesList) {
        val name1 = iface.class1.name
        val name2 = iface.class2.name
        if (!interfaces.containsKey(name1))
          interfaces.put(name1, new HashMap<String, Interface>())
        if (!interfaces.containsKey(name2))
          interfaces.put(name2, new HashMap<String, Interface>())
        interfaces.get(name1).put(name2, iface)
        interfaces.get(name2).put(name1, iface)
      }
      return interfaces
    }
  
}