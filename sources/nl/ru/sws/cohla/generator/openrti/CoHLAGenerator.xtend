/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

package nl.ru.sws.cohla.generator.openrti

import java.util.Arrays
import java.util.Collections
import java.util.List
import nl.ru.sws.cohla.coHLA.DSEConfig
import nl.ru.sws.cohla.coHLA.Distribution
import nl.ru.sws.cohla.coHLA.Environment
import nl.ru.sws.cohla.coHLA.FaultScenario
import nl.ru.sws.cohla.coHLA.FederateObject
import nl.ru.sws.cohla.coHLA.FederateType
import nl.ru.sws.cohla.coHLA.Federation
import nl.ru.sws.cohla.coHLA.Interface
import nl.ru.sws.cohla.coHLA.MetricSet
import nl.ru.sws.cohla.coHLA.ModelConfiguration
import nl.ru.sws.cohla.coHLA.Scenario
import nl.ru.sws.cohla.coHLA.Situation
import nl.ru.sws.cohla.generator.openrti.config.DSEGenerator
import nl.ru.sws.cohla.generator.openrti.config.DistributionGenerator
import nl.ru.sws.cohla.generator.openrti.config.FaultScenarioGenerator
import nl.ru.sws.cohla.generator.openrti.config.MetricGenerator
import nl.ru.sws.cohla.generator.openrti.config.ScenarioGenerator
import nl.ru.sws.cohla.generator.openrti.config.SituationGenerator
import nl.ru.sws.cohla.generator.openrti.config.TopologyGenerator
import nl.ru.sws.cohla.generator.openrti.metric.MetricFederateGenerator
import nl.ru.sws.cohla.generator.openrti.metric.MetricFederateHeaderGenerator
import nl.ru.sws.cohla.generator.openrti.metric.MetricFederateMainGenerator
import nl.ru.sws.cohla.generator.openrti.wrapper.CMakeGenerator
import nl.ru.sws.cohla.generator.openrti.wrapper.FederateGenerator
import nl.ru.sws.cohla.generator.openrti.wrapper.FederateHeaderGenerator
import nl.ru.sws.cohla.generator.openrti.wrapper.MainGenerator
import nl.ru.sws.cohla.generator.openrti.wrapper.SimulatorGenerator
import nl.ru.sws.cohla.generator.openrti.wrapper.SimulatorHeaderGenerator
import org.eclipse.xtext.generator.IFileSystemAccess2
import org.eclipse.xtext.generator.IGeneratorContext

class CoHLAGenerator {
  
  static def doGenerate(IFileSystemAccess2 fsa, IGeneratorContext context, Environment env, 
    List<FederateObject> federateObjects, List<Federation> federations, String projectName,
    List<ModelConfiguration> modelConfigurations, List<Interface> interfaces) {
      val hasMetrics = federations.exists[f | !f.metricSets.empty];
      val srcDir = projectName + "/" + (env.srcDir ?: "src") 
      for (f : federateObjects.filter[e | e.type == FederateType.FMU || e.type == FederateType.POOSL || e.type == FederateType.COLSIM]) {
              val fedBaseName = srcDir + "/" + f.name.toFirstUpper + "/" + f.name.toFirstUpper
              fsa.generateFile(fedBaseName + "Simulator.h", SimulatorHeaderGenerator.generate(f))
              fsa.generateFile(fedBaseName + "Simulator.cpp", SimulatorGenerator.generate(f))
      }
      for (o : federateObjects) {
          val objectBaseName = srcDir + "/" + o.name.toFirstUpper + "/" + o.name.toFirstUpper
          fsa.generateFile(objectBaseName + ".cpp", MainGenerator.generate(o))
          fsa.generateFile(objectBaseName + "Federate.h", FederateHeaderGenerator.generate(o))
          fsa.generateFile(objectBaseName + "Federate.cpp", FederateGenerator.generate(o, federateObjects, hasMetrics, env.printLevel, env.publishOnlyChanges))
      }
      if (hasMetrics) {
          val metricAttributes = federations.map[f | f.metricSets].flatten.map[s | s.metrics].flatten.map[s | if (s.metricEV !== null) {
            if (s.metricEV.relativeTo !== null) Arrays.asList(s.metricEV.attribute, s.metricEV.relativeTo.ref)
            else Collections.singletonList(s.metricEV.attribute)
          } else if (s.metricErr !== null) {
            if (s.metricErr.relativeTo !== null) Arrays.asList(s.metricErr.attribute, s.metricErr.relativeTo.ref)
            else Collections.singletonList(s.metricErr.attribute)
          } else if (s.metricTimer !== null) {
            Collections.singletonList(s.metricTimer.attribute)
          } else if (s.metricMinMax !== null) {
            Collections.singletonList(s.metricMinMax.attribute)
          }].flatten.toList
          fsa.generateFile(srcDir + "/Metrics/MetricApplication.cpp", MetricFederateMainGenerator.generate())
          fsa.generateFile(srcDir + "/Metrics/MetricCollector.cpp", MetricFederateGenerator.generate(metricAttributes, federateObjects))
          fsa.generateFile(srcDir + "/Metrics/MetricCollector.h", MetricFederateHeaderGenerator.generate())
        }
      for (modelConf : modelConfigurations)
          fsa.generateFile(projectName + "/conf/" + modelConf.federateObject.name.toFirstUpper + "/" + modelConf.name + ".init", SituationGenerator.generate(modelConf))
      fsa.generateFile(srcDir + "/CMakeLists.txt", CMakeGenerator.generate(federateObjects, projectName, env.rti, hasMetrics))
      fsa.generateFile(projectName + "/run.py", RunScriptGenerator.generate(federateObjects, federations, env))
      for (federation : federations) {
        fsa.generateFile(projectName + "/conf/" + federation.name.toFirstLower + ".topo", TopologyGenerator.generate(federation, interfaces))
        for (Situation situation: federation.situations)
          fsa.generateFile(projectName + "/conf/" + federation.name.toFirstUpper + "/" + situation.name + ".situation", SituationGenerator.generate(situation));
        for (Scenario scenario : federation.scenarios)
          fsa.generateFile(projectName + "/conf/" + federation.name.toFirstUpper + "/" + scenario.name + ".scenario", ScenarioGenerator.generate(scenario));
        for (FaultScenario scenario : federation.faultScenarios)
          fsa.generateFile(projectName + "/conf/" + federation.name.toFirstUpper + "/" + scenario.name + ".fscenario", FaultScenarioGenerator.generate(scenario));
        for (DSEConfig dse : federation.dses)
          fsa.generateFile(projectName + "/conf/" + federation.name.toFirstUpper + "/" + dse.name + ".dse", DSEGenerator.generate(dse));
        for (MetricSet metricSet : federation.metricSets)
          fsa.generateFile(projectName + "/conf/" + federation.name.toFirstUpper + "/" + metricSet.name + ".metrics", MetricGenerator.generate(metricSet));
        for (Distribution distribution : federation.distributions)
          fsa.generateFile(projectName + "/conf/" + federation.name.toFirstUpper + "/" + distribution.name + ".distribution", DistributionGenerator.generate(distribution))
      }
  }
  
}