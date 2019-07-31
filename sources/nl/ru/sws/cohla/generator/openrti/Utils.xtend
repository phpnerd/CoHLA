/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

package nl.ru.sws.cohla.generator.openrti

import java.util.Arrays
import java.util.Collections
import java.util.function.Function
import nl.ru.sws.cohla.coHLA.Attribute
import nl.ru.sws.cohla.coHLA.DataType
import nl.ru.sws.cohla.coHLA.FederateInstance
import nl.ru.sws.cohla.coHLA.FederateObject
import nl.ru.sws.cohla.coHLA.FederateType
import nl.ru.sws.cohla.coHLA.LoggerConfig
import nl.ru.sws.cohla.coHLA.MultiInputOperator
import nl.ru.sws.cohla.coHLA.Parameter
import nl.ru.sws.cohla.generator.openrti.models.ClassArgument

class Utils {
  
  static def toVarDecl(Attribute attr, boolean isGetter, String... prepName)
        '''«toVarType(attr.dataType)» «IF prepName.length > 0»«prepName.get(0)»«ENDIF»«IF isGetter»get«attr.name.toFirstUpper»()«ELSE»«attr.name.toFirstLower»«ENDIF»'''
        
  static def toVarType(DataType type) {
    switch (type) {
      case BOOL: '''bool'''
      case INTEGER: '''int'''
      case LONG: '''long'''
      case REAL: '''double'''
      case STRING: '''string'''
      default: '''void*'''
    }
  }
    
    static def toDefaultDecl(Attribute attr)
        '''«attr.name.toFirstLower»(«switch (attr.dataType) {
            case BOOL: '''false'''
            case INTEGER: '''0'''
            case LONG: '''0'''
            case REAL: '''0.0'''
            case STRING: '''""'''
            default: ''''''
        }»)'''
        
    static def toFMUSimGetter(DataType dataType) {
      switch(dataType) {
        case BOOL: '''getBool'''
        case INTEGER: '''getInt'''
        case LONG: '''getInt'''
        case REAL: '''getReal'''
        case STRING: '''getString'''
      }
    }
    
    static def toOperator(MultiInputOperator op) {
      switch (op) {
        case AND: '''&&'''
        case OR: '''||'''
        case PLUS: '''+'''
        case PRODUCT: '''*'''
        default: ''''''
      }
    }
    
    static def toSimGetter(Attribute attr, FederateType type) {
        switch (type) {
            case FMU:
'''try {
    «attr.name.toFirstLower» = ''' + toFMUSimGetter(attr.dataType) + '''(«attr.name.toUpperCase»_NAME);
} catch (string& errorMessage) {
    wcerr << "Error: " << errorMessage.c_str() << endl;
}'''
            case POOSL: {
                attr.name.toFirstLower + ''' = ''' + stringToType("getElement(process" + attr.processProperty.process.name.toFirstUpper + ", " + attr.name.toUpperCase + "_NAME).getLiteral()", attr.dataType) + ''';'''
            }
            case COLSIM: {
              if (attr.collision)
                '''«attr.name.toFirstLower» = CollisionSimulator::getCollisions()«IF attr.dataType == DataType.BOOL» > 0«ENDIF»;'''
              else
                '''«attr.name.toFirstLower» = getState(«attr.name.toUpperCase»_NAME);'''
            }
            default: '''// TODO: Insert simulation getter for «attr.name»!'''
        }
    }
    
    static def toFMUSimSetter(DataType dataType) {
      switch(dataType) {
          case BOOL: '''setBool'''
          case INTEGER: '''setInt'''
          case LONG: '''setInt'''
          case REAL: '''setReal'''
          case STRING: '''setString'''
      }
    }
    
    static def toSimSetter(Attribute attr, FederateType type) {
        val vName = attr.name.toFirstLower
        switch (type) {
            case FMU: 
'''try {
    «toFMUSimSetter(attr.dataType)»(«attr.name.toUpperCase»_NAME, «vName»);
} catch (string& errorMessage) {
    wcerr << "Error: " << errorMessage.c_str() << endl;
}'''
            case POOSL: '''setElement(process«attr.processProperty.process.name.toFirstUpper», «attr.processProperty.process.name.toUpperCase»_PATH, «attr.name.toUpperCase»_NAME, «vName»)'''
            case COLSIM: '''setState(«attr.name.toUpperCase»_NAME, «vName»)'''
            default: '''// TODO: Insert simulation setter for «attr.name»!'''
        }
    }
    
    static def stringToType(String s, DataType type) {
        switch (type) {
            case BOOL: '''«s» == "true"'''
            case INTEGER: '''stoi(«s»)'''
            case LONG: '''stol(«s»)'''
            case REAL: '''stod(«s»)'''
            case STRING: '''"«s»"''' 
        }
    }
    
    static def toDefaultStep(FederateObject f, boolean isFirstStep)
      '''«switch (f.advanceType) {
      case TIME: '''advanceTime'''
      case MESSAGE: '''advanceNextMessage'''
    }»(«switch (f.type) { 
      case POOSL: '''sim->proceed()'''
      default: '''stepSize«IF isFirstStep» - fed->federateLookahead«ENDIF»'''
    }»)'''
    
    static def printCounter(int from, int to, CharSequence separator, Function<Integer, String> fun) {
      val s = new StringBuilder();
      for (var i = from; i < to; i++) {
        if (i > from)
          s.append(separator);
        s.append(fun.apply(i));
      }
      return s.toString();
    }
    
    static def toAttributeName(Parameter param) {
      val name = new StringBuilder();
      param.name.split("( |\\.)").forEach[s | name.append(s.toFirstUpper)];
      return name.toString;
    }
    
    static def toBashInitAttributeList(FederateInstance federateInstance) 
    '''«FOR i : federateInstance.federate.parameters» $«federateInstance.name.toFirstLower»Init«toAttributeName(i)»«ENDFOR»'''
    
    static def toCppInitAttributeList(FederateObject federateObject)
    '''«federateObject.parameters.join(" ", " ", "", [initVar | "[param" + toAttributeName(initVar) + "]"])»'''
    
    static def getSimulatorTypeArguments(FederateObject fed) {
      switch (fed.type) {
        case CSV: return Arrays.asList(
          new ClassArgument("file", "Sets the output log file to X.", "\" + logDir + \"" + fed.name.toFirstUpper + "\" + \".csv"), 
          new ClassArgument("measureTime", "Sets the measure time to X", (fed.config as LoggerConfig).defaultMeasureTime ?: "3600.0")
        )
        case FMU: return Arrays.asList(
          new ClassArgument("model", "Sets the model to be executed to X", fed.defaultModel.get(0) ?: "../../" + fed.name.toFirstUpper + ".fmu"),
          new ClassArgument("tmpDir", "Sets the temporary directory to X", "/tmp/" + fed.name.toFirstUpper),
          new ClassArgument("name", "Sets the model name to X", fed.name),
          new ClassArgument("stepSize", "Sets the step size to X", fed.defaultStepSize ?: "1.0"),
          new ClassArgument("lookahead", "Sets the lookahead to X", fed.defaultLookahead ?: "1.0"))
        case POOSL: return Arrays.asList(
          new ClassArgument("model", "Sets the model to be executed to X", fed.defaultModel.get(0) ?: "../../" + fed.name.toFirstUpper + ".poosl"),
          new ClassArgument("hostname", "Sets the hostname for Rotalumis to X", "127.0.0.1"),
          new ClassArgument("port", "Sets the port for Rotalumis to X", "10001"),
          new ClassArgument("lookahead", "Sets the lookahead to X", fed.defaultLookahead ?: "1.0"))
        case COLSIM: return Arrays.asList(
          new ClassArgument("models", "Sets the collision models to X..", "[ " + (if (!fed.defaultModel.empty) fed.defaultModel.map[s | "'" + s + "'"].join(", ") else fed.name.toFirstUpper + ".json") + " ]", false),
          new ClassArgument("appendModel", "Appends the collision model X", "[]", false),
          new ClassArgument("render", "If True, a render display will be opened", "False", false),
          new ClassArgument("lookahead", "Sets the lookahead to X", fed.defaultLookahead ?: "1.0"))
        case NONE: return Arrays.asList(
          new ClassArgument("stepSize", "Sets the step size to X", fed.defaultStepSize ?: "1.0"),
          new ClassArgument("lookahead", "Sets the lookahead to X", fed.defaultLookahead ?: "1.0"))
      }
    }
    
    static def getInitialisableArguments(FederateObject fedObj) {
      switch(fedObj.type) {
        case FMU: return fedObj.parameters.map[p | new ClassArgument(p.name, "Sets the initial " + p.name + " to X", "nil")]
        case POOSL: return fedObj.parameters.map[p | new ClassArgument(p.name, "Sets the initial " + p.name + " to X", "nil")]
        default: return Collections.emptyList
      }
    }
  
}