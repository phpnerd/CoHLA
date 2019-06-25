/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

package nl.ru.sws.cohla.generator.openrti.wrapper

import java.util.List
import nl.ru.sws.cohla.coHLA.FederateObject
import nl.ru.sws.cohla.coHLA.FederateType
import nl.ru.sws.cohla.coHLA.HLAImplementation

class CMakeGenerator {
  
  static def generate(List<FederateObject> federateObjects, String projectName, HLAImplementation hlaImp, boolean hasMetric) '''
«val requiresPooslLib = federateObjects.exists[f | f.type == FederateType.POOSL]»
«val requiresFmuLib = federateObjects.exists[f | f.type == FederateType.FMU]»
«val requiresCsvLib = federateObjects.exists[f | f.type == FederateType.CSV]»
«val requiresColSimLib = federateObjects.exists[f | f.type == FederateType.COLSIM]»
cmake_minimum_required(VERSION 3.5)
project(«projectName» C CXX)

macro(get_WIN32_WINNT version)
  if (WIN32 AND CMAKE_SYSTEM_VERSION)
    set(ver ${CMAKE_SYSTEM_VERSION})
    string(REPLACE "." "" ver ${ver})
    string(REGEX REPLACE "([0-9])" "0\\1" ver ${ver})
    set(${version} "0x${ver}")
  endif()
endmacro()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_STANDARD 14)

IF(WIN32)
  set(DEPENDENCY_ROOT "«hlaImp.depRoot ?: "C:/opt"»" CACHE PATH "Root directory where the dependency libraries are installed")
  set(BOOST_ROOT ${DEPENDENCY_ROOT}/boost CACHE PATH "Installation directory of Boost")
  get_WIN32_WINNT(WIN_VERSION)
  add_definitions(-D_WIN32_WINNT=${WIN_VERSION})
  add_definitions( -DBOOST_ALL_DYN_LINK )
  set(Boost_USE_STATIC_LIBS        OFF)
  set(Boost_USE_MULTITHREADED      ON)
  set(Boost_USE_STATIC_RUNTIME     OFF)
  
  set(WIN_SOCKET_LIB ws2_32)
  set(FMILIB_NAME fmilib_shared)
  set(GLFW_NAME glfw3dll)
ELSE(WIN32)
  set(DEPENDENCY_ROOT "«hlaImp.depRoot ?: "/usr/local"»" CACHE PATH "Root directory where the dependency libraries are installed")
  set(FMILIB_NAME fmilib)
  set(GLFW_NAME glfw)
ENDIF(WIN32)

link_directories(${DEPENDENCY_ROOT}/lib)
find_package(Boost REQUIRED COMPONENTS system thread)
link_directories(${Boost_LIBRARY_DIRS})

«IF requiresColSimLib»find_package(OpenGL)
find_package(Bullet)«ENDIF»
«IF requiresPooslLib»find_package(XercesC)«ENDIF»
find_library(RTI1516e rti1516e)
find_library(FMIlib ${FMILIB_NAME})
find_library(GLFW ${GLFW_NAME})
find_library(HACD HACD)

SET(ORTI_LIB_ROOT "«hlaImp.libRoot ?: "${DEPENDENCY_ROOT}"»")
SET(ORTI_LIB_INCLUDES ${ORTI_LIB_ROOT}/include)

link_directories(${ORTI_LIB_ROOT}/lib)
find_library(ORTIL_rti rti HINTS ${ORTI_LIB_ROOT}/lib)
find_library(ORTIL_socket socket HINTS ${ORTI_LIB_ROOT}/lib)
«IF requiresCsvLib»find_library(ORTIL_csv csv HINTS ${ORTI_LIB_ROOT}/lib)«ENDIF»
«IF requiresFmuLib»find_library(ORTIL_fmu fmu HINTS ${ORTI_LIB_ROOT}/lib)«ENDIF»
«IF requiresPooslLib»find_library(ORTIL_poosl poosl HINTS ${ORTI_LIB_ROOT}/lib)«ENDIF»
«IF requiresColSimLib»find_library(ORTIL_collision collision HINTS ${ORTI_LIB_ROOT}/lib)«ENDIF»
«IF hasMetric»find_library(ORTIL_metrics metrics HINTS ${ORTI_LIB_ROOT}/lib)«ENDIF»

include_directories(
    ${DEPENDENCY_ROOT}/include
    ${DEPENDENCY_ROOT}/include/rti1516e
    ${Boost_INCLUDE_DIRS}
    ${ORTI_LIB_INCLUDES}/libortiutils
    ${ORTI_LIB_INCLUDES}/librti
    ${ORTI_LIB_INCLUDES}/librti/include
    ${ORTI_LIB_INCLUDES}/libsocket
    «IF requiresPooslLib»${ORTI_LIB_INCLUDES}/libpoosl«ENDIF»
    «IF requiresFmuLib»${ORTI_LIB_INCLUDES}/libfmu«ENDIF»
    «IF requiresCsvLib»${ORTI_LIB_INCLUDES}/libcsv«ENDIF»
    «IF requiresColSimLib»${ORTI_LIB_INCLUDES}/libcollision
    ${ORTI_LIB_INCLUDES}/libcollision/include
    ${BULLET_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIRS}«ENDIF»
    «IF hasMetric»${ORTI_LIB_INCLUDES}/libmetrics«ENDIF»
)

set(CMAKE_BINARY_DIR bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
«FOR f : federateObjects»

# Federate «f.name.toFirstUpper»
file(GLOB_RECURSE FED_«f.name.toUpperCase»_SOURCE_FILES «f.name.toFirstUpper»/*)
add_executable(«f.name.toFirstLower» ${FED_«f.name.toUpperCase»_SOURCE_FILES})
target_link_libraries(«f.name.toFirstLower» ${ORTIL_rti} ${RTI1516e} ${ORTIL_socket} ${WIN_SOCKET_LIB} ${Boost_LIBRARIES} «switch (f.type) { case FMU: '''${ORTIL_fmu} ${FMIlib}''' case POOSL: '''${ORTIL_poosl} ${XercesC_LIBRARY}''' case CSV: '''${ORTIL_csv}''' case COLSIM: '''${ORTIL_collision} ${GLFW} ${BULLET_LIBRARIES} ${OpenGL_LIBRARIES} ${HACD}''' default: ''''''}»)
«ENDFOR»
«IF hasMetric»

# MetricFederate
file(GLOB_RECURSE METRIC_SOURCE_FILES Metrics/*)
add_executable(metricProcessor ${METRIC_SOURCE_FILES})
target_link_libraries(metricProcessor ${ORTIL_rti} ${RTI1516e} ${ORTIL_socket} ${WIN_SOCKET_LIB} ${Boost_LIBRARIES} ${ORTIL_metrics})
«ENDIF»

install(TARGETS«FOR o : federateObjects» «o.name.toFirstLower»«ENDFOR»
        RUNTIME DESTINATION bin)
    '''
  
}
