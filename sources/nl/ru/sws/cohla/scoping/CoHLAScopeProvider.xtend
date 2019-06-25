/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

package nl.ru.sws.cohla.scoping

import java.util.Arrays
import nl.ru.sws.cohla.coHLA.AttributeReference
import nl.ru.sws.cohla.coHLA.ClassAttributeReference
import nl.ru.sws.cohla.coHLA.Federation
import nl.ru.sws.cohla.coHLA.InstanceParameterReference
import nl.ru.sws.cohla.coHLA.Interface
import nl.ru.sws.cohla.coHLA.InterfaceConnection
import nl.ru.sws.cohla.coHLA.ModelConfiguration
import nl.ru.sws.cohla.coHLA.ParameterReference
import nl.ru.sws.cohla.coHLA.Situation
import org.eclipse.emf.ecore.EObject
import org.eclipse.emf.ecore.EReference
import org.eclipse.xtext.scoping.Scopes

import static nl.ru.sws.cohla.coHLA.CoHLAPackage.Literals.*

class CoHLAScopeProvider extends AbstractCoHLAScopeProvider {

  override getScope(EObject context, EReference reference) {
    if (context instanceof AttributeReference)          return getScope(context, reference)
    if (context instanceof Situation)                   return getScope(context, reference)
    if (context instanceof InstanceParameterReference)  return getScope(context, reference)
    if (context instanceof ModelConfiguration)          return getScope(context, reference)
    if (context instanceof ParameterReference)          return getScope(context, reference)
    if (context instanceof InterfaceConnection)         return getScope(context, reference)
    if (context instanceof ClassAttributeReference)     return getScope(context, reference)
    return super.getScope(context, reference)
  }
  
  def getScope(AttributeReference ref, EReference reference) {
    if (reference == ATTRIBUTE_REFERENCE__ATTRIBUTE)
      if (ref.instance !== null && ref.instance.federate !== null && ref.instance.federate.attributes !== null)
        return Scopes.scopeFor(ref.instance.federate.attributes)
    return super.getScope(ref, reference)
  }
  
  def getScope(Situation situation, EReference reference) {
    if (reference == INSTANCE_PARAMETER_REFERENCE__INSTANCE || reference == APPLY_MODEL_CONFIGURATION__INSTANCE)
      return Scopes.scopeFor((situation.eContainer as Federation).instances)
    if (reference == SITUATION__EXTENDS) {
      val federation = situation.eContainer as Federation
      return Scopes.scopeFor(federation.situations.filter[s | s != situation])
    }
    return super.getScope(situation, reference)
  }
  
  def getScope(InstanceParameterReference ref, EReference reference) {
    if (reference == INSTANCE_PARAMETER_REFERENCE__ATTRIBUTE)
      if (ref.instance !== null && ref.instance.federate !== null && ref.instance.federate.parameters !== null)
        return Scopes.scopeFor(ref.instance.federate.parameters)
    return super.getScope(ref, reference)
  }
  
  def getScope(ModelConfiguration modelConf, EReference reference) {
    if (reference == PARAMETER_REFERENCE__ATTRIBUTE)
      return Scopes.scopeFor(modelConf.federateObject.parameters)
    return super.getScope(modelConf, reference)
  }
  
  def getScope(ParameterReference ref, EReference reference) {
    if (reference == PARAMETER_REFERENCE__ATTRIBUTE) {
      val fedObj = (ref.eContainer.eContainer as ModelConfiguration).federateObject
      return Scopes.scopeFor(fedObj.parameters)
    }
    return super.getScope(ref, reference)
  }
  
  def getScope(InterfaceConnection interfaceConnection, EReference ref) {
    if (ref == CLASS_ATTRIBUTE_REFERENCE__OBJECT_CLASS) {
      val iface = interfaceConnection.eContainer as Interface
      val o1 = iface.class1
      val o2 = iface.class2
      return Scopes.scopeFor(Arrays.asList(o1, o2))
    }
    return super.getScope(interfaceConnection, ref)
  }
  
  def getScope(ClassAttributeReference classAttributeRef, EReference ref) {
    if (classAttributeRef.eContainer instanceof InterfaceConnection) {
      if (ref == CLASS_ATTRIBUTE_REFERENCE__OBJECT_CLASS) {
        val iface = classAttributeRef.eContainer.eContainer as Interface
        val o1 = iface.class1
        val o2 = iface.class2
        return Scopes.scopeFor(Arrays.asList(o1, o2))
      } else if (ref == CLASS_ATTRIBUTE_REFERENCE__ATTRIBUTE) {
        return Scopes.scopeFor(classAttributeRef.objectClass.attributes)
      }
    }
    return super.getScope(classAttributeRef, ref);
  }

}
