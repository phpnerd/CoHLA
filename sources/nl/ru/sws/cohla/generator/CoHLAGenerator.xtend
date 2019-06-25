/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

package nl.ru.sws.cohla.generator

import nl.ru.sws.cohla.ResourceUtils
import nl.ru.sws.cohla.coHLA.Environment
import nl.ru.sws.cohla.coHLA.FederateObject
import nl.ru.sws.cohla.coHLA.Federation
import nl.ru.sws.cohla.coHLA.Import
import nl.ru.sws.cohla.coHLA.Interaction
import nl.ru.sws.cohla.coHLA.Interface
import nl.ru.sws.cohla.coHLA.Model
import nl.ru.sws.cohla.coHLA.ModelConfiguration
import org.eclipse.emf.ecore.resource.Resource
import org.eclipse.xtext.generator.AbstractGenerator
import org.eclipse.xtext.generator.IFileSystemAccess2
import org.eclipse.xtext.generator.IGeneratorContext


class CoHLAGenerator extends AbstractGenerator {

	override void doGenerate(Resource resource, IFileSystemAccess2 fsa, IGeneratorContext context) {
        var env = resource.allContents.findLast[e | e instanceof Environment] as Environment;
        val federateObjects = resource.allContents.filter[e | e instanceof FederateObject].map[e | e as FederateObject].toList;
        val configurations = resource.allContents.filter[e | e instanceof ModelConfiguration].map[e | e as ModelConfiguration].toList;
        val interactions = resource.allContents.filter[e | e instanceof Interaction].map[e | e as Interaction].toList;
        val federations = resource.allContents.filter[e | e instanceof Federation].map[e | e as Federation].toList;
        val imports = resource.allContents.filter[e | e instanceof Import].map[e | e as Import].toList;
        val modelConfigurations = resource.allContents.filter[e | e instanceof ModelConfiguration].map[e | e as ModelConfiguration].toList;
        val interfaces = resource.allContents.filter(Interface).toList
        for (Import i : imports) {
          val importedModel = ResourceUtils.resourceToEObject(ResourceUtils.openImport(i.eResource, i.importURI)) as Model
          if (importedModel.environment !== null) {
            if (env === null)
              env = importedModel.environment
            else
              println("Skipping environment from imported \"" + i.importURI + "\" due to HLAEnvironment definition in main source.")
          }
          if (importedModel.federateObjects !== null && !importedModel.federateObjects.empty)
            federateObjects.addAll(importedModel.federateObjects)
          if (importedModel.configurations !== null && !importedModel.configurations.empty)
            configurations.addAll(importedModel.configurations)
          if (importedModel.interactions !== null && !importedModel.interactions.empty)
            interactions.addAll(importedModel.interactions)
          if (importedModel.federations !== null && !importedModel.federations.empty)
            federations.addAll(importedModel.federations)
          if (importedModel.configurations !== null && !importedModel.configurations.empty)
            modelConfigurations.addAll(importedModel.configurations)
          if (importedModel.interfaces !== null && !importedModel.interfaces.empty)
            interfaces.addAll(importedModel.interfaces)
        }
        if (env === null || federateObjects.empty || federations.empty) {
          println("Skipping generation")
          return;
        }
        val projectName = resource.URI.trimFileExtension.lastSegment.toFirstUpper;
        fsa.generateFile(projectName + "/fom.xml", FOMGenerator.generate(federateObjects, interactions, projectName, federations.exists[f | !f.metricSets.empty]));
        switch env.rti.implementation {
          case OPEN_RTI: {
            println("Generating code for OpenRTI..")
            nl.ru.sws.cohla.generator.openrti.CoHLAGenerator.doGenerate(fsa, context, env, federateObjects, federations, projectName, modelConfigurations, interfaces)
            println("Done!")
        }
        case PITCH_RTI: {
          println("Code generation for Pitch RTI is not yet supported!")
        }
        case PORTICO: {
          println("Code generation for PoRTIco is not yet supported!")
        }
        default: {
          println("Undefined HLAVersion to generate for.")
        }
        }
    }
}
