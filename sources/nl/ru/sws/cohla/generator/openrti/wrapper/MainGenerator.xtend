/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

package nl.ru.sws.cohla.generator.openrti.wrapper

import java.util.Collections
import nl.ru.sws.cohla.coHLA.FederateObject
import nl.ru.sws.cohla.coHLA.FederateType

import static nl.ru.sws.cohla.generator.openrti.Utils.*

class MainGenerator {
  
  static def generate(FederateObject f) '''
«val uName = f.name.toFirstUpper»
«val lName = f.name.toFirstLower»
#include <iostream>
#include <cstdlib>
#include <csignal>
#include "../«uName»/«uName»Federate.h"

using namespace std;

«uName»Federate* «lName»Federate;

void signal_handler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        «lName»Federate->stop();
        delete «lName»Federate;
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    if (signal(SIGINT, signal_handler) == SIG_ERR)
        wcerr << "Failed to register signal_handler for SIGINT" << endl;
    if (signal(SIGTERM, signal_handler) == SIG_ERR)
        wcerr << "Failed to register signal handler for SIGTERM" << endl;
    
    «IF f.type == FederateType.POOSL»
    «val nrOfArguments = 9 + (f.parameters ?: Collections.emptyList).length»
    if (argc < «nrOfArguments + 1») {
        cout << "Invalid number of arguments; expected «nrOfArguments»: [topology] [federateName] [pooslModel] [hostname] [port] [lookahead] [rtiAddress] [scenario | nil] [faultScenatio | nil]«toCppInitAttributeList(f)»" << endl;
        return -1;
    }
    string topology = argv[1];
    string federateName = argv[2];
    string pooslModel = argv[3];
    string hostname = argv[4];
    int port = stoi(argv[5]);
    double lookahead = stod(argv[6]);
    string rtiAddress = argv[7];
    string scenario = argv[8];
    string faultScenario = argv[9];
    
    «lName»Federate = new «uName»Federate(pooslModel, hostname, port);
    «IF nrOfArguments > 9»
    «lName»Federate->initParameters(«printCounter(10, nrOfArguments + 1, ", ", [argNr | "argv[" + argNr + "]"])»);
    «ENDIF»
    «ELSEIF f.type == FederateType.FMU»
    «val nrOfArguments = 10 + (f.parameters ?: Collections.emptyList).length»
    if (argc < «nrOfArguments + 1») {
        wcout << "Invalid number of arguments; expected «nrOfArguments»: [topology] [federateName] [fmuModel] [tmpDir] [modelName] [stepSize] [lookahead] [rtiAddress] [scenario | nil] [faultScenario | nil]«toCppInitAttributeList(f)»" << endl;
        return -1;
    }
    
    string topology = argv[1];
    string federateName = argv[2];
    string fmuModel = argv[3];
    string tmpDir = argv[4];
    string modelName = argv[5];
    double stepSize = stod(argv[6]);
    double lookahead = stod(argv[7]);
    string rtiAddress = argv[8];
    string scenario = argv[9];
    string faultScenario = argv[10];
    
    «uName»Federate* «lName»Federate = new «uName»Federate(fmuModel, tmpDir, modelName, stepSize);
    «IF nrOfArguments > 10»
    «lName»Federate->initParameters(«printCounter(11, nrOfArguments + 1, ", ", [argNr | "argv[" + argNr + "]"])»);
    «ENDIF»
    «ELSEIF f.type == FederateType.COLSIM»
    if (argc < 9) {
        cout << "Invalid number of arguments; expected 8 or more: [topology] [federateName] [collisionModel]... [do_render] [lookahead] [rtiAddress] [scenario | nil] [faultScenario | nil]" << endl;
        return -1;
    }
    string topology = argv[1];
    string federateName = argv[2];
    vector<string> collisionModels;
    for (int i = 3; i < argc - 5; i++)
        collisionModels.emplace_back(argv[i]);
    unsigned long nrOfModels = collisionModels.size();
    string renderStr = string(argv[nrOfModels + 3]);
    bool do_render = renderStr == "true" || renderStr == "True" || renderStr == "TRUE";
    double lookahead = stod(argv[nrOfModels + 4]);
    string rtiAddress = argv[nrOfModels + 5];
    string scenario = argv[nrOfModels + 6];
    string faultScenario = argv[nrOfModels + 7];
    
    «lName»Federate = new «uName»Federate(collisionModels, do_render);
    «ELSEIF f.type == FederateType.CSV»
    if (argc < 7) {
        cout << "Invalid number of arguments; expected 6 or 7: [topology] [federateName] [csvFile] [measureTime?] [rtiAddress] [scenario | nil] [faultScenario | nil]" << endl;
        return -1;
    }
    string topology = argv[1];
    string federateName = argv[2];
    string csvFile = argv[3];
    string rtiAddress = argv[4 + (argc > 7 ? 1 : 0)];
    double lookahead = 0.0;
    string scenario = argv[5 + (argc > 7 ? 1 : 0)];
    string faultScenario = argv[6 + (argc > 7 ? 1 : 0)];
    
    if (argc > 5)
        «lName»Federate = new «uName»Federate(csvFile, stod(argv[4]));
    else
        «lName»Federate = new «uName»Federate(csvFile);
    «ELSE»
    if (argc < 8) {
        cout << "Invalid number of arguments; expected 7: [topology] [federateName] [stepSize] [lookahead] [rtiAddress] [scenario | nil] [faultScenario | nil]" << endl;
        return -1;
    }
    string topology = argv[1];
    string federateName = argv[2];
    double stepSize = stod(argv[3]);
    double lookahead = stod(argv[4]);
    string rtiAddress = argv[5];
    string scenario = argv[6];
    string faultScenario = argv[7];

    «lName»Federate = new «uName»Federate(stepSize);
    «ENDIF»
    «lName»Federate->setConfig(federateName, "fom.xml", rtiAddress, topology, scenario, faultScenario);
    ((federate*)«lName»Federate)->init(«switch (f.timePolicy) {
      case BOTH: '''true, lookahead'''
      case CONSTRAINED: '''true'''
      case REGULATED: '''false, lookahead'''
      default: '''false'''
    }»);
    «lName»Federate->run();
    «lName»Federate->stop();
    delete «lName»Federate;
    return 0;
}
    '''
  
}
