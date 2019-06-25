/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

package nl.ru.sws.cohla.generator

import java.util.List
import nl.ru.sws.cohla.coHLA.Attribute
import nl.ru.sws.cohla.coHLA.FederateObject
import nl.ru.sws.cohla.coHLA.Interaction
import nl.ru.sws.cohla.coHLA.ParameterProperty

import static nl.ru.sws.cohla.generator.HLAUtils.*

class FOMGenerator {
  
  static def generate(List<FederateObject> federateObjects, List<Interaction> interactions, String name, boolean hasMetrics) '''
<?xml version='1.0' encoding='utf-8'?>
<objectModel 
 xmlns='http://standards.ieee.org/IEEE1516-2010'
 xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'
 xsi:schemaLocation='http://standards.ieee.org/IEEE1516-2010 http://standards.ieee.org/downloads/1516/1516.2-2010/IEEE1516-DIF-2010.xsd'>
  <modelIdentification>
      <name>«name»</name>
      <type>FOM</type>
      <useLimitation>NA</useLimitation>
  </modelIdentification>
  <objects>
    <objectClass>
      <name>HLAobjectRoot</name>
      <sharing>Neither</sharing>
      <attribute>
        <name>HLAprivilegeToDeleteObject</name>
        <dataType>HLAtoken</dataType>
        <updateType>Static</updateType>
        <updateCondition>NA</updateCondition>
        <ownership>DivestAcquire</ownership>
        <sharing>PublishSubscribe</sharing>
        <transportation>HLAreliable</transportation>
        <order>TimeStamp</order>
      </attribute>
      «federateObjects.map[generate].join('\n')»
      «IF hasMetrics»
      <objectClass>
        <name>MetricProcessor</name>
        <sharing>Subscribe</sharing>
      </objectClass>
      «ENDIF»
    </objectClass>
  </objects>
  <interactions>
    <interactionClass>
      <name>HLAinteractionRoot</name>
      <sharing>Neither</sharing>
      <transportation>HLAreliable</transportation>
      <order>TimeStamp</order>
      «interactions.map[generate].join('\n')»
    </interactionClass>
  </interactions>
  <dimensions/>
  <time>
    <timeStamp/>
    <lookahead/>
  </time>
  <tags>
    <updateReflectTag/>
    <sendReceiveTag/>
    <deleteRemoveTag/>
    <divestitureRequestTag/>
    <divestitureCompletionTag/>
    <acquisitionRequestTag/>
    <requestUpdateTag/>
  </tags>
  <synchronizations/>
  <transportations>
    <transportation>
      <name>HLAreliable</name>
      <reliable>Yes</reliable>
      <semantics>Provide reliable delivery of data in the sense that TCP/IP delivers its data reliably</semantics>
    </transportation>
    <transportation>
      <name>HLAbestEffort</name>
      <reliable>No</reliable>
      <semantics>Make an effort to deliver data in the sense that UDP provides best-effort delivery</semantics>
    </transportation>
  </transportations>
  <updateRates/>
  <switches>
    <autoProvide isEnabled="false" />
    <conveyRegionDesignatorSets isEnabled="false" />
    <conveyProducingFederate isEnabled="false" />
    <attributeScopeAdvisory isEnabled="false" />
    <attributeRelevanceAdvisory isEnabled="false" />
    <objectClassRelevanceAdvisory isEnabled="false" />
    <interactionRelevanceAdvisory isEnabled="false" />
    <serviceReporting isEnabled="false" />
    <exceptionReporting isEnabled="false"/>
    <delaySubscriptionEvaluation isEnabled="false"/>
    <automaticResignAction resignAction="NoAction" />
  </switches>
  <dataTypes>
    <basicDataRepresentations/>
    <simpleDataTypes/>
    <enumeratedDataTypes/>
    <arrayDataTypes/>
    <fixedRecordDataTypes/>
    <variantRecordDataTypes/>
  </dataTypes>
  <notes/>
</objectModel>
    '''
    
    static def generate(FederateObject f)
'''<objectClass>
  <name>«f.name»</name>
  <sharing>«toHlaSharingType(f)»</sharing>
  «f.attributes.map[generate].join('\n')»
</objectClass>'''

  static def generate(Attribute a)
'''<attribute>
  <name>«a.name»</name>
  <dataType>«toHla(a.dataType)»</dataType>
  <updateType>«toHla(a.updateTypeProperty)»</updateType>
  <updateCondition>«toHla(a.updateConditionProperty)»</updateCondition>
  <ownership>NoTransfer</ownership>
  <sharing>«toHlaSharingType(a.sharingType)»</sharing>
  <transportation>«toHla(a.transportationProperty)»</transportation>
  <order>«toHla(a.orderProperty)»</order>
  <semantics>N/A</semantics>
</attribute>'''

  static def generate(Interaction i)
'''<interactionClass>
  <name>«i.name»</name>
  <sharing>«i.sharingProperty.sharing»</sharing>
  <transportation>«i.transportationProperty.transportation»</transportation>
  <order>«i.orderProperty.order»</order>
  «i.parameters.map[generate].join('\n')»
</interactionClass>'''

  static def generate(ParameterProperty p)
'''<parameter>
  <name>«p.name»</name>
  <dataType>«toHla(p.dataType.dataType)»</dataType>
  <semantics>N/A</semantics>
</parameter>'''
  
}
