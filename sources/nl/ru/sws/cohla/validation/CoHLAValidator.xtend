/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

package nl.ru.sws.cohla.validation

import nl.ru.sws.cohla.coHLA.Attribute
import nl.ru.sws.cohla.coHLA.AttributeConnection
import nl.ru.sws.cohla.coHLA.DataType
import nl.ru.sws.cohla.coHLA.FederateInstance
import nl.ru.sws.cohla.coHLA.FederateObject
import nl.ru.sws.cohla.coHLA.FederateType
import nl.ru.sws.cohla.coHLA.Federation
import nl.ru.sws.cohla.coHLA.LoggerConnection
import nl.ru.sws.cohla.coHLA.Model
import nl.ru.sws.cohla.coHLA.POOSLConfig
import nl.ru.sws.cohla.coHLA.SharingType
import nl.ru.sws.cohla.coHLA.TimePolicy
import org.eclipse.xtext.validation.Check

import static nl.ru.sws.cohla.coHLA.CoHLAPackage.Literals.*

/**
 * This class contains custom validation rules. 
 *
 * See https://www.eclipse.org/Xtext/documentation/303_runtime_concepts.html#validation
 */
class CoHLAValidator extends AbstractCoHLAValidator {
  
  @Check
  def checkFederateObject(FederateObject federate) {
    val type = federate.type;
    if (type != FederateType.POOSL) {
      if (federate.config !== null && federate.config instanceof POOSLConfig) {
        warning("Incompatible configuration for FederateClass type!",
          FEDERATE_OBJECT__CONFIG
        )
      }
    }
    else if (type == FederateType.POOSL && (federate.config === null || !(federate.config instanceof POOSLConfig) || (federate.config as POOSLConfig).processes.empty)) {
      warning("Processes are usually required when running a Rotalumis simulation",
        FEDERATE_OBJECT__NAME
      )
    }
    if (type == FederateType.CSV || type == FederateType.COLSIM) {
      if (federate.timePolicy != TimePolicy.BOTH)
        warning("TimePolicy is not a valid property for a " + type.literal + " federate", FEDERATE_OBJECT__TIME_POLICY);
      if (federate.defaultStepSize !== null)
        warning("DefaultStepSize is not a valid property for a " + type.literal + " federate", FEDERATE_OBJECT__DEFAULT_STEP_SIZE);
    }
    if (type == FederateType.CSV) {
      if (!federate.defaultModel.empty)
        warning("DefaultModel is not a valid property for a CSV federate", FEDERATE_OBJECT__DEFAULT_MODEL);
      if (federate.defaultLookahead !== null)
        warning("DefaultLookahead is not a valid property for a CSV federate", FEDERATE_OBJECT__DEFAULT_LOOKAHEAD);
      if (federate.attributes !== null && !federate.attributes.empty)
        warning("CSV simulator does not require pre-defined attributes.", FEDERATE_OBJECT__ATTRIBUTES);
    }
    if (type == FederateType.COLSIM) {
      if (!federate.attributes.exists[a | a.collision])
        warning("No collision attribute specified for a BulletCollision federate", FEDERATE_OBJECT__NAME);
    }
    if (!(type == FederateType.FMU || type == FederateType.POOSL) && federate.parameters !== null && !federate.parameters.empty) {
      error("Parameters are currently only supported for FMU simulators",
        FEDERATE_OBJECT__PARAMETERS
      )
    }
    if (type == FederateType.FMU || type == FederateType.POOSL) {
      if (federate.defaultModel.length > 1)
        error("Only one default model is supported!", FEDERATE_OBJECT__DEFAULT_MODEL);
    }
  }
  
  @Check
  def checkFederateAttribute(Attribute attribute) {
    val federate = attribute.eContainer as FederateObject
    val type = federate.type
    if (type != FederateType.POOSL && attribute.processProperty !== null) {
      error("Only federates with simulator type Rotalumis are allowed to have a process property",
        ATTRIBUTE__NAME
      )
    }
  }
  
  @Check
  def checkDoubleAttributeNames(Attribute attribute) {
    val federate = attribute.eContainer as FederateObject
    if (federate.attributes.findFirst[a | a != attribute && a.name.equals(attribute.name)] !== null)
      error("Attribute " + attribute.name + " is already defined", ATTRIBUTE__NAME)
  }
  
  @Check
  def checkCollisionAttribute(Attribute attribute) {
    if (attribute.collision) {
      val federate = attribute.eContainer as FederateObject
      if (federate.type != FederateType.COLSIM)
        error("Collision flag can only be set for attributes in a Collision federate", ATTRIBUTE__COLLISION)
      if (attribute.dataType != DataType.INTEGER && attribute.dataType != DataType.BOOL)
        error("Collision flag can only be set for Integer or Boolean attributes", ATTRIBUTE__DATA_TYPE)
      if (attribute.sharingType == SharingType.PUBLISH_SUBSCRIBE || attribute.sharingType == SharingType.SUBSCRIBE)
        error("Collision attribute cannot be of type Input or InOutput", ATTRIBUTE__SHARING_TYPE)
    }
  }
  
  @Check
  def checkDoubleFederateObjectNames(FederateObject federate) {
    val federateObjects = (federate.eContainer as Model).federateObjects
    if (federateObjects.exists[f | f != federate && f.name.equals(federate.name)])
      error("FederateObject " + federate.name + " is defined multiple times", FEDERATE_OBJECT__NAME)
  }
  
  @Check
  def checkDoubleFederateInstanceNames(FederateInstance instance) {
    val federation = instance.eContainer as Federation
    if (federation.instances.findFirst[i | i != instance && i.name.equals(instance.name)] !== null)
      error("FederateInstance " + instance.name + " is defined multiple times", FEDERATE_INSTANCE__NAME)
  }
  
  @Check
  def checkAttributeConnection(AttributeConnection connection) {
    val out = connection.outAttributes
    val in = connection.inAttribute
    val outAttrs = out.map[oa | oa.attribute]
    val inAttr = in.attribute
    if (inAttr.sharingType == SharingType.PUBLISH || inAttr.sharingType == SharingType.NEITHER)
      error("Input attribute is not defined as an input attribute!", ATTRIBUTE_CONNECTION__IN_ATTRIBUTE)
    if (outAttrs.exists[oa | oa.dataType != inAttr.dataType])
      warning("Input and output attribute data type should be equal.", ATTRIBUTE_CONNECTION__IN_ATTRIBUTE)
    if (outAttrs.exists[oa | oa.sharingType == SharingType.SUBSCRIBE || oa.sharingType == SharingType.NEITHER])
      error("Output attribute is not defined as an output attribute!", ATTRIBUTE_CONNECTION__OUT_ATTRIBUTES)
    val federation = connection.eContainer as Federation
    out.forEach[outAttr |
      if (federation.connections.filter(AttributeConnection).exists[c | c != connection &&
          c.inAttribute.instance == in.instance && c.inAttribute.attribute == in.attribute && 
          c.outAttributes.exists[oa | oa.instance == outAttr.instance && oa.attribute == outAttr.attribute]
        ]) {
        warning("Connection already exists", ATTRIBUTE_CONNECTION__IN_ATTRIBUTE)
        warning("Connection already exists", ATTRIBUTE_CONNECTION__OUT_ATTRIBUTES)
      }
    ]
    if (federation.connections.filter(AttributeConnection).exists[c | 
      out.exists[oa | oa.instance == c.inAttribute.instance && oa.attribute == c.inAttribute.attribute] &&
      c.outAttributes.exists[oa | in.instance == oa.instance && in.attribute == oa.attribute]
    ]) {
      warning("Circular connection detected", ATTRIBUTE_CONNECTION__IN_ATTRIBUTE)
      warning("Circular connection detected", ATTRIBUTE_CONNECTION__OUT_ATTRIBUTES)
    }
  }
  
  @Check
  def checkLoggerConnection(LoggerConnection connection) {
    val out = connection.outAttributes
    val outAttrs = out.map[oa | oa.attribute]
    if (outAttrs.exists[oa | oa.sharingType == SharingType.SUBSCRIBE || oa.sharingType == SharingType.NEITHER])
      error("Output attribute is not defined as an output attribute!", LOGGER_CONNECTION__OUT_ATTRIBUTES)
  }
  
}
