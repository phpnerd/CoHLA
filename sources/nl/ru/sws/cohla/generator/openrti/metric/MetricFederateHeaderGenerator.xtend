/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

package nl.ru.sws.cohla.generator.openrti.metric

class MetricFederateHeaderGenerator {
  
  static def generate() '''
#ifndef METRICSFEDERATE_H
#define METRICSFEDERATE_H

#include <MetricProcessor.h>
#include <limits>

class MetricCollector : public MetricProcessor {
public:
    MetricCollector(string, string, double measureTime);
    virtual ~MetricCollector();

protected:

private:
    void initialiseHandles();
};

#endif //METRICSFEDERATE_H
  '''
  
}
