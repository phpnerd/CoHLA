/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

package nl.ru.sws.cohla.generator.openrti.config

import nl.ru.sws.cohla.coHLA.Distribution

class DistributionGenerator {
  
  static def generate(Distribution dist)
'''
#systems:«dist.nrOfSystems»
«FOR system : dist.systemSets»
«FOR federate : system.federates»
«federate.name.toFirstUpper»:«system.systemId»
«ENDFOR»
«ENDFOR»
'''
  
}
