/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

package nl.ru.sws.cohla

import org.eclipse.emf.common.util.URI
import org.eclipse.emf.ecore.EObject
import org.eclipse.emf.ecore.resource.Resource
import org.eclipse.emf.ecore.resource.ResourceSet

class ResourceUtils {
  
  public def static EObject resourceToEObject(Resource resource) {
    return resource?.allContents?.head;
  }
  public def static Resource openImport(Resource currentResource, String importedURIAsString) {
    val URI currentURI = currentResource?.getURI;
    val URI importedURI = URI?.createURI(importedURIAsString);
    val URI resolvedURI = importedURI?.resolve(currentURI);
    val ResourceSet currentResourceSet = currentResource?.resourceSet;
    val Resource resource = currentResourceSet?.getResource(resolvedURI, true);
    return resource;
  }
  
}
