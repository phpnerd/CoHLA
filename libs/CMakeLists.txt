cmake_minimum_required(VERSION 3.5)
project(OpenRTI_lib)

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

if(WIN32)
	set(DEPENDENCY_ROOT C:/opt CACHE PATH "Root directory where the dependency libraries are installed")
	set(BOOST_ROOT ${DEPENDENCY_ROOT}/boost CACHE PATH "Installation directory of Boost")
	get_WIN32_WINNT(WIN_VERSION)
	add_definitions(-D_WIN32_WINNT=${WIN_VERSION})
	add_definitions( -DBOOST_ALL_DYN_LINK )
	set(Boost_USE_STATIC_LIBS OFF)
	set(Boost_USE_MULTITHREADED ON)
	set(Boost_USE_STATIC_RUNTIME OFF)
	set(LIB_SOCKET ws2_32)
	set(ADD_RTI_LIB ortiutils socket)
	set(FMILIB_NAME fmilib_shared)
	set(GLFW_NAME glfw3dll)
else(WIN32)
	set(DEPENDENCY_ROOT /usr/local CACHE PATH "Root directory where the dependency libraries are installed")
	set(FMILIB_NAME fmilib)
	set(GLFW_NAME glfw)
endif(WIN32)

link_directories(${DEPENDENCY_ROOT}/lib)
find_package(Boost REQUIRED COMPONENTS system thread)
link_directories(${Boost_LIBRARY_DIRS})

find_package(XercesC)
find_package(Bullet)
find_library(RTI1516e rti1516e)
find_library(FedTime1516e fedtime1516e)
find_library(FMIlib ${FMILIB_NAME})
find_library(GLFW ${GLFW_NAME})
find_library(HACD HACD)

if(APPLE)
	find_library(COCOA_LIBRARY Cocoa)
	find_library(IOKIT_LIBRARY IOKit)
	find_library(COREVIDEO_LIBRARY CoreVideo)
	set(MAC_GLFW_LIBS ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY})
	set(ADD_RTI_LIB socket)
endif(APPLE)

message(STATUS "FMI-lib    : " ${FMIlib})
message(STATUS "RTI-lib    : " ${RTI1516e})
message(STATUS "RTItime-lib: " ${FedTime1516e})
message(STATUS "GLFW-lib   : " ${GLFW})

option(BUILD_TESTS "Whether to build or not to build all test executables." OFF)
if(BUILD_TESTS)
	set(BUILD_SOCKET_TEST ON)
	set(BUILD_COLLISION_TEST ON)
endif()

set(ORTI_UTIL_DIR ${PROJECT_SOURCE_DIR}/utils)
file(GLOB_RECURSE UTIL_SOURCE_FILES ${ORTI_UTIL_DIR}/*)
file(GLOB UTIL_HEADER_FILES ${ORTI_UTIL_DIR}/*.h*)
add_library(ortiutils INTERFACE)
target_sources(ortiutils INTERFACE ${UTIL_SOURCE_FILES})
target_include_directories(ortiutils INTERFACE include)

option(BUILD_SOCKET_TEST "Whether to build or not to build the socket test executable." OFF)
set(SOCKET_SOURCE_DIR ${PROJECT_SOURCE_DIR}/socket)
file(GLOB_RECURSE SOCKET_SOURCE_FILES ${SOCKET_SOURCE_DIR}/socket.cpp)
file(GLOB_RECURSE SOCKET_HEADER_FILES ${SOCKET_SOURCE_DIR}/socket.h)
add_library(socket ${SOCKET_SOURCE_FILES})
target_link_libraries(socket ${Boost_LIBRARIES} ${LIB_SOCKET})
if(BUILD_SOCKET_TEST)
	add_executable(socket_test ${SOCKET_SOURCE_DIR}/test.cpp)
	target_link_libraries(socket_test socket pthread ${Boost_LIBRARIES})
	set(EXTRA_TARGETS ${EXTRA_TARGETS} socket_test)
endif()
target_include_directories(socket PUBLIC include)

set(RTI_SOURCE_DIR ${PROJECT_SOURCE_DIR}/rti)
file(GLOB_RECURSE RTILIB_SOURCE_FILES ${RTI_SOURCE_DIR}/*)
file(GLOB RTILIB_HEADER_FILES ${RTI_SOURCE_DIR}/*.h*)
add_library(rti ${RTILIB_SOURCE_FILES})
target_link_libraries(rti ${RTI1516e} ${FedTime1516e} ${ADD_RTI_LIB})
target_include_directories(rti PUBLIC include)

set(POOSL_SOURCE_DIR ${PROJECT_SOURCE_DIR}/poosl)
file(GLOB_RECURSE POOSL_SOURCE_FILES ${POOSL_SOURCE_DIR}/*)
file(GLOB_RECURSE POOSL_HEADER_FILES ${POOSL_SOURCE_DIR}/*.h*)
add_library(poosl ${POOSL_SOURCE_FILES})
target_link_libraries(poosl socket ${XercesC_LIBRARY})
target_include_directories(poosl PUBLIC include)

set(FMU_SOURCE_DIR ${PROJECT_SOURCE_DIR}/fmu)
file(GLOB_RECURSE FMU_SOURCE_FILES ${FMU_SOURCE_DIR}/*)
file(GLOB_RECURSE FMU_HEADER_FILES ${FMU_SOURCE_DIR}/*.h*)
add_library(fmu ${FMU_SOURCE_FILES})
target_link_libraries(fmu ${FMIlib})
target_include_directories(fmu PUBLIC include)

set(CSV_SOURCE_DIR ${PROJECT_SOURCE_DIR}/csv_export)
file(GLOB_RECURSE CSV_SOURCE_FILES ${CSV_SOURCE_DIR}/*)
file(GLOB_RECURSE CSV_HEADER_FILES ${CSV_SOURCE_DIR}/*.h*)
add_library(csv ${CSV_SOURCE_FILES})
target_link_libraries(csv rti)
target_include_directories(csv PUBLIC include)

set(METRIC_SOURCE_DIR ${PROJECT_SOURCE_DIR}/metric)
file(GLOB_RECURSE METRIC_SOURCE_FILES ${METRIC_SOURCE_DIR}/*)
file(GLOB_RECURSE METRIC_HEADER_FILES ${METRIC_SOURCE_DIR}/*.h*)
add_library(metrics ${METRIC_SOURCE_FILES})
target_link_libraries(metrics rti)
target_include_directories(metrics PUBLIC include)

option(BUILD_COLLISION_TEST "Whether to build or not to build the collision test executable." OFF)
set(COLLISION_SOURCE_DIR ${PROJECT_SOURCE_DIR}/collision)
file(GLOB_RECURSE COLLISION_SOURCE_FILES ${COLLISION_SOURCE_DIR}/*.c*)
list(REMOVE_ITEM COLLISION_SOURCE_FILES ${COLLISION_SOURCE_DIR}/test.cpp)
file(GLOB COLLISION_HEADER_FILES ${COLLISION_SOURCE_DIR}/*.h*)
add_library(collision ${COLLISION_SOURCE_FILES})
target_link_libraries(collision ${BULLET_LIBRARIES} ${Boost_LIBRARIES} ${GLFW} ${HACD} ${MAC_GLFW_LIBS})
if(BUILD_COLLISION_TEST)
	add_executable(collision_test ${COLLISION_SOURCE_DIR}/test.cpp)
	target_link_libraries(collision_test collision ${HACD})
	set(EXTRA_TARGETS ${EXTRA_TARGETS} collision_test)
endif()
target_include_directories(collision PUBLIC include)

include_directories(
		${DEPENDENCY_ROOT}/include/rti1516e
		${DEPENDENCY_ROOT}/include
        ${PROJECT_SOURCE_DIR}
		${ORTI_UTIL_DIR}
        ${SOCKET_SOURCE_DIR}
        ${RTI_SOURCE_DIR}
		${RTI_SOURCE_DIR}/include
        ${POOSL_SOURCE_DIR}
        ${FMU_SOURCE_DIR}
        ${CSV_SOURCE_DIR}
		${METRIC_SOURCE_DIR}
        ${COLLISION_SOURCE_DIR}
        ${COLLISION_SOURCE_DIR}/include
        ${BULLET_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
)

install(TARGETS ortiutils socket rti poosl fmu csv collision metrics ${EXTRA_TARGETS}
		RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
install(FILES ${SOCKET_HEADER_FILES} DESTINATION include/libsocket)
install(FILES ${RTILIB_HEADER_FILES} DESTINATION include/librti)
install(DIRECTORY ${RTI_SOURCE_DIR}/include/models ${RTI_SOURCE_DIR}/include/scenario ${RTI_SOURCE_DIR}/include/faults DESTINATION include/librti FILES_MATCHING PATTERN "*.h*")
install(FILES ${POOSL_HEADER_FILES} DESTINATION include/libpoosl)
install(FILES ${FMU_HEADER_FILES} DESTINATION include/libfmu)
install(FILES ${CSV_HEADER_FILES} DESTINATION include/libcsv)
install(FILES ${METRIC_HEADER_FILES} DESTINATION include/libmetrics)
install(FILES ${COLLISION_HEADER_FILES} DESTINATION include/libcollision)
install(DIRECTORY ${COLLISION_SOURCE_DIR}/include DESTINATION include/libcollision FILES_MATCHING PATTERN "*.h*")
install(FILES ${UTIL_HEADER_FILES} DESTINATION include/libortiutils)
