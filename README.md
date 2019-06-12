# CoHLA
Configuring HLA (CoHLA) is a domain specific language to rapidly construct HLA-based co-simulations from a set of models.
CoHLA was introduced on the Conference on Industrial Cyber-Physical Systems (ICPS) 2018 in Saint Petersburg [[1]](https://thomasnagele.nl/papers/ICPS2018/).
CoHLA was created using [Xtext](https://www.eclipse.org/Xtext/) and [Xtend](https://www.eclipse.org/xtend/) for [Eclipse](https://www.eclipse.org/).

**Disclaimer** The sources included in this project are provided as-is. These sources are currently still lacking proper comments and documentation. This may be added in the future. Also, this README is known to be incomplete.

## Structure
The structure of this repository is as follows.

* **cohla** contains the sources of the DSL itself. At this time, these are still being refactored, so we'll commit them as soon as the refactoring has finished.
* **docs** contains the documentation regarding both the installation of CoHLA and all of its dependencies as well as a manual on how to use CoHLA and generate code from it.
* **libs** contains the C++ libraries that have been developed for use with [OpenRTI](https://sourceforge.net/projects/openrti/). CoHLA generates code for use with these libraries.

## Installation
When the sources will be available, the CoHLA DSL project can be imported by creating a new Xtext project having the name ```nl.ru.ws.cohla``` with extension "cohla".

Until then, the CoHLA extension for Eclipse can be installed by adding the repository as software source to Eclipse.
The repository location is [https://files.thomasnagele.nl/cohla/plugin/](https://files.thomasnagele.nl/cohla/plugin/).
After having added this repository, the latest version of CoHLA can be installed from it.

To actually run the project, the OpenRTI-libraries located in the *libs* folder should be installed.

The CoHLA framework was mostly tested on Ubuntu 18.04.
