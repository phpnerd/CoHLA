# OpenRTI-libraries
This (sub)project provides the sources of a set of libraries that was developed for use with the [OpenRTI](https://sourceforge.net/projects/openrti/) implementation of the High Level Architecture.
In particular, these libraries are used by the code that is generated by CoHLA.
The libraries are provided in a regular CMake project.

**Disclaimer** The sources included in this project are provided as-is. These sources are currently still lacking proper comments and documentation. This may be added in the future. Also, this README is known to be incomplete.

## Dependencies
The project relies on the following dependencies.

* [CMake](https://cmake.org/) 3.5 or higher
* [Boost](https://www.boost.org/) (system, thread)
* [Xerces-C](https://xerces.apache.org/xerces-c/)
* [GLM](https://glm.g-truc.net/0.9.9/index.html) 0.9+
* [GLFW](http://www.glfw.org/) 3.2.1
* [OpenRTI](https://sourceforge.net/projects/openrti/) 0.9
* [FMI-library](https://jmodelica.org/) 2.0.3
* [Bullet Physics Library](http://bulletphysics.org/wordpress/) 2.86
* [CodeSynthesis XSD](https://www.codesynthesis.com/products/xsd/)

For running POOSL simulations, the [Rotalumis](http://www.es.ele.tue.nl/poosl/Tools/rotalumis/) simulator is also required.

## Installation
An installation manual is provided in *docs* folder in the root of the project.
Provided that all dependencies are already installed, this project can be built using the following commands.

```
mkdir build
cd build
cmake -DBUILD_SHARED_LIBS=ON ..
make
make install
```

### Ubuntu 18.04
For Ubuntu 18.04, an installation script is provided that downloads, builds and installs all dependencies, after which the OpenRTI-libraries are installed as well.
This script is located in the root of this (sub)project and should be executed as superuser.

## Contents
The following libraries are included.

* **RTI** (*rti*): Provides a federate wrapper with a default implementation of a federate.
* **Socker** (*socket*): Provides a basic socket implementation that is used for scenarios and the POOSL library.
* **Logger** (*csv_export*): Provides a logger federate implementation with default methods to store and write log (CSV) files.
* **MetricCollector** (*metric*): Provides a type of logger federate that outputs metric results instead of a CSV file. It requires a set of metrics as input.
* **FMU** (*fmu*): The FMU connector library for wrapping Functional Mockup Units.
* **POOSL** (*poosl*): The POOSL connector library for wrapping POOSL models. This library connects to an already running instance of Rotalumis.
