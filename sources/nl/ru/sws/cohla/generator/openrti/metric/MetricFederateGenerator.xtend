/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

package nl.ru.sws.cohla.generator.openrti.metric

import java.util.List
import nl.ru.sws.cohla.coHLA.AttributeReference
import nl.ru.sws.cohla.coHLA.FederateObject

class MetricFederateGenerator {
  
  static def generate(List<AttributeReference> references, List<FederateObject> federateObjects) '''
#include "MetricCollector.h"

MetricCollector::MetricCollector(string metricFile, string outputPath, double measureTime = numeric_limits<double>::max()):
    MetricProcessor::MetricProcessor(metricFile, outputPath, measureTime) {}

MetricCollector::~MetricCollector() = default;

void MetricCollector::initialiseHandles() {
    «FOR obj : federateObjects»
    objectClassHandles["«obj.name»"] = rti->getObjectClassHandle(L"HLAobjectRoot.«obj.name»");
    «FOR attr : obj.attributes»
    attributeHandles["«obj.name»"]["«attr.name»"] = rti->getAttributeHandle(objectClassHandles["«obj.name»"], L"«attr.name»");
    «ENDFOR»
    «ENDFOR»
    objectClassHandles["MetricProcessor"] = rti->getObjectClassHandle(L"HLAobjectRoot.MetricProcessor");
    objectClassHandle = objectClassHandles["MetricProcessor"];
    wcout << "Initialised handles" << endl;
}
  '''
  
}
