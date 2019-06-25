/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

package nl.ru.sws.cohla.generator.openrti.config

import nl.ru.sws.cohla.coHLA.AttributeReference
import nl.ru.sws.cohla.coHLA.Metric
import nl.ru.sws.cohla.coHLA.MetricEndValue
import nl.ru.sws.cohla.coHLA.MetricError
import nl.ru.sws.cohla.coHLA.MetricMinMax
import nl.ru.sws.cohla.coHLA.MetricRelativeTo
import nl.ru.sws.cohla.coHLA.MetricSet
import nl.ru.sws.cohla.coHLA.MetricTimer

import static nl.ru.sws.cohla.generator.openrti.Utils.*

class MetricGenerator {
  
  static def generate(MetricSet metricSet) '''
metrics:«metricSet.metrics.size»
measuretime:«metricSet.measureTime»
«metricSet.metrics.map[generate].join("\n")»'''

  static def generate(Metric metric) '''«generateMetric(metric)»'''
  
  static def generateMetric(Metric metric) {
    if (metric.metricEV !== null)
      generate(metric, metric.metricEV)
    else if (metric.metricErr !== null)
      generate(metric, metric.metricErr)
    else if (metric.metricTimer !== null)
      generate(metric, metric.metricTimer)
    else if (metric.metricMinMax !== null) {
      generate(metric, metric.metricMinMax)
    }
  }
  
  static def generate(Metric m, MetricEndValue d) 
  '''«toVarType(d.attribute.attribute.dataType).toString.toUpperCase»:EV:«m.name»:«IF d.absolute»ABSOLUTE«ELSE»NONE«ENDIF»:«toAttrRef(d.attribute)»:«toRelative(d.relativeTo)»'''
  
  static def generate(Metric m, MetricError d) 
  '''«toVarType(d.attribute.attribute.dataType).toString.toUpperCase»:ERR:«m.name»:«IF d.squared»SQUARED«ELSE»NONE«ENDIF»:«toAttrRef(d.attribute)»:«toAttrRef(d.relativeTo.ref)»'''
  
  static def generate(Metric m, MetricTimer d)
  '''«toVarType(d.attribute.attribute.dataType).toString.toUpperCase»:TIMER:«m.name»:«toAttrRef(d.attribute)»:«d.comparator»:«d.value»:«d.isIsEndCondition»:«d.delay ?: 0.0»'''
  
  static def generate(Metric m, MetricMinMax d)
  '''«toVarType(d.attribute.attribute.dataType).toString.toUpperCase»:«IF d.min»MIN«ELSEIF d.max»MAX«ELSE»ERROR«ENDIF»:«m.name»:«toAttrRef(d.attribute)»''' 
  
  static def toRelative(MetricRelativeTo relTo) '''REL_«IF relTo === null»SELF:«ELSE»OTHER:«toAttrRef(relTo.ref)»«ENDIF»'''
  
  static def toAttrRef(AttributeReference ref) '''«ref.instance.name.toFirstUpper».«ref.attribute.name»'''
  
}
