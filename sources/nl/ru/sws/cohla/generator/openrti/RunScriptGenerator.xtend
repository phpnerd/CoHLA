/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

package nl.ru.sws.cohla.generator.openrti

import java.util.List
import nl.ru.sws.cohla.coHLA.Environment
import nl.ru.sws.cohla.coHLA.FederateObject
import nl.ru.sws.cohla.coHLA.FederateType
import nl.ru.sws.cohla.coHLA.Federation
import nl.ru.sws.cohla.generator.openrti.models.ClassArgument

class RunScriptGenerator {
  
  static def generate(List<FederateObject> federateObjects, List<Federation> feds, Environment env) '''
#!/usr/bin/python3

import os, sys, subprocess, time, ast, signal
from argparse import ArgumentParser
from collections import OrderedDict
from functools import reduce
from itertools import product
from operator import attrgetter

readableTime = time.strftime("%m/%d/%Y %H:%M:%S")
startTime = time.strftime("%Y%m%d%H%M%S")

arguments = None
federationName = ""
instances = OrderedDict()
connections = []
situation = OrderedDict()
dse = OrderedDict()
dseMetricResults = OrderedDict()
logDir = "logs/" + startTime + "/"
rotalumisPort = 10001
rotalumisExecutable = "rotalumis"

rtinodeCommand = "rtinode"

GUI = None
runSem = True

running = building = stop = instancesInitialised = False

startTimeStamp = 0
processes = []
simulations = []
loggers = []
metricProcessors = []

class FederateClass:
  def __init__(self, fedName, fedClass, topology, rtiHostname, additionalCommand=None, additionalCommandEnv=None, isLogger=False, isMetricProcessor=False, doStart=True, simWeight=1):
    self.topology = topology
    self.rtiHostname = rtiHostname
    self.fedName = fedName
    self.fedClass = fedClass
    self.additionalCommand = additionalCommand
    self.additionalCommandEnv = additionalCommandEnv
    self.args = {}
    self.initArgs = {}
    self.help = {}
    self.initHelp = {}
    self.argKeyOrder = []
    self.initArgKeyOrder = []
    self.isLogger = isLogger
    self.isMetricProcessor = isMetricProcessor
    self.doStart = doStart
    self.simWeight = simWeight
  
  def __repr__(self):
    return self.fedName + " ( " + self.fedClass + " )"
  
  def getCommand(self):
    return os.path.join("build", "bin", lower(self.fedClass)) + " \"" + self.topology + "\" " + (("\"" + self.fedName + "\" ") if not self.isMetricProcessor else "") + " ".join([((" ".join(["\"" + str(subArg) + "\"" for subArg in self.args[key]])) if isinstance(self.args[key], list) else ("\"" + str(self.args[key]) + "\"")) for key in self.argKeyOrder]) + " " + self.rtiHostname + " " + self.getScenarioArguments() + " " + " ".join([("\"" + str(self.initArgs[key]) + "\"") for key in self.initArgKeyOrder])
  
  def getScenarioArguments(self):
      return "\"" + (arguments.scenario if arguments.scenario is not None else "nil") + "\" \"" + (arguments.faultScenario if arguments.faultScenario is not None else "nil") + "\""
  
  def getAdditionalCommand(self):
    return self.additionalCommand
  
  def getAdditionalCommandEnv(self):
    return self.additionalCommandEnv
  
  def applySituation(self):
    if self.fedName in situation:
      for initValue in situation[self.fedName].items():
        self.initArgs[initValue[0]] = initValue[1]
  
  def collectArguments(self):
    parser = ArgumentParser(add_help=False)
    n = lower(self.fedName)
    for arg in self.args.keys():
      if isinstance(self.args[arg], list):
        parser.add_argument("--" + n + upper(arg), dest=arg, default=None, action='append')
      else:
        parser.add_argument("--" + n + upper(arg), dest=arg, default=None)
    args, _ = parser.parse_known_args()
    for arg in vars(args).items():
      if arg[1] is not None:
        self.args[arg[0]] = arg[1]
    parser = ArgumentParser(add_help=False)
    for arg in self.initArgs.keys():
      parser.add_argument("--" + n + "Init" + upper(arg), dest=arg, default=None)
    args, _ = parser.parse_known_args()
    for arg in vars(args).items():
      if arg[1] is not None:
        self.initArgs[arg[0]] = arg[1]
    
  def printConfig(self):
    return """
=== INSTANCE """ + self.fedName + """ ===
Name           : """ + self.fedName + """
Class          : """ + self.fedClass + """
AutoStart      : """ + str(self.doStart) + """
Sim Weight     : """ + str(self.simWeight) + """
""" + "\n".join(map(lambda arg: upper(arg[0]).ljust(15) + ": " + str(arg[1]), self.args.items())) + ("" if len(self.initArgs) == 0 else """
""" + "\n".join(map(lambda arg: upper(arg[0]).ljust(15) + ": " + str(arg[1]), self.initArgs.items()))) + """
Command        : """ + self.getCommand() + ("""
Extra command  : """ + self.getAdditionalCommand() if self.getAdditionalCommand() is not None else """""")

class NoneFederateClass(FederateClass):
  def __init__(self, fedName, topology, rtiHostname, additionalArgument=None):
    super().__init__(fedName, "None", topology, rtiHostname, additionalArgument)

«federateObjects.map[generateFederateClass].join("\n\n")»

class MetricProcessor(FederateClass):
  def __init__(self, topology, rtiHostname, metricFile=None, outputFile=logDir + "MetricResults.txt", measureTime="3600.0", additionalCommand=None):
    super().__init__("MetricProcessor", "MetricProcessor", topology, rtiHostname, additionalCommand, isMetricProcessor=True)
    self.args = { "metricFile": metricFile, "outputFile": outputFile, "measureTime": measureTime }
    self.help = { "metricFile": ("Sets the file containing the metrics to X.", None), "outputFile": ("Sets the output metric file to X.", logDir + "MetricResults.txt"), "measureTime": ("Sets the measure time to X.", "3600.0") }
    self.argKeyOrder = [ "metricFile", "outputFile", "measureTime" ]

class Connection:
  def __init__(self, targetInstance, targetAttribute, sourceInstance, sourceAttribute):
    self.targetInstance = targetInstance
    self.targetAttribute = targetAttribute
    self.sourceInstance = sourceInstance
    self.sourceAttribute = sourceAttribute

class Distribution:
  def __init__(self, nrOfSets):
    self.sets = OrderedDict({ id: DistributionSet(id) for id in range(nrOfSets) })
  
  def __repr__(self):
    return repr(self.sets)
  
  def add(self, instance):
    if instance.isLogger or instance.isMetricProcessor:
      self.sets[0].add(instance)
      return
    idx = min(self.sets.values(), key=attrgetter('totalWeight')).systemId
    self.sets[idx].add(instance)
  
  def addAll(self, instances):
    for instance in instances.values():
      self.add(instance)
  
  def getInstances(self, systemId):
    return self.sets[systemId].instances

class DistributionSet:
  def __init__(self, systemId):
    self.systemId = systemId
    self.totalWeight = 0
    self.instances = OrderedDict()
  
  def __repr__(self):
    return repr((self.systemId, self.totalWeight, self.instances))
  
  def add(self, instance):
    self.totalWeight += instance.simWeight
    self.instances[instance.fedName] = instance

def main():
  global arguments, GUI
  parser = ArgumentParser(add_help=False)
  parser.add_argument("-t", "--topology", dest="topology", metavar="FILE", default=None)
  parser.add_argument("-b", "--build", dest="build", action="store_true", default=False)
  parser.add_argument("-f", "--fom", dest="fom", metavar="FILE", default="fom.xml")
  parser.add_argument("-e", "--execute", dest="execute", action="store_true", default=False)
  parser.add_argument("-d", "--display", dest="display", action="store_true", default=False)
  parser.add_argument("-r", "--rti", dest="rti", metavar="STRING", default="localhost")
  parser.add_argument("-h", "--help", dest="help", action="store_true", default=False)
  parser.add_argument("-s", "--situation", dest="situations", action="append", nargs="+", default=[])
  parser.add_argument("-u", "--scenario", dest="scenario", metavar="FILE", default=None)
  parser.add_argument("--faults", dest="faultScenario", metavar="FILE", default=None)
  parser.add_argument("--dse", dest="dse", metavar="FILE", default=None)
  parser.add_argument("-m", "--metrics", dest="metrics", metavar="FILE", default=None)
  parser.add_argument("-v", "--verbose", dest="verbose", action="store_true", default=False)
  parser.add_argument("--render", dest="render", action="store_true", default=False)
  parser.add_argument("--cmake", dest="cmake", metavar="STRING", default=None)
  parser.add_argument("--disable", dest="disable", default=[], action="append")
  
  parser.add_argument("--parentHost", dest="parentRtiHost", metavar="Hostname", default=None)
  parser.add_argument("--parentPort", dest="parentRtiPort", metavar="Port", default=14323)
  parser.add_argument("--nrOfSystems", dest="nrOfSystems", metavar="Number of systems", default=1)
  parser.add_argument("--systemId", dest="systemId", metavar="System ID", default=0)
  parser.add_argument("--distribution", dest="distributionConfig", metavar="FILE", default=None)
  args, _ = parser.parse_known_args()
  args.nrOfSystems = int(args.nrOfSystems)
  args.systemId = int(args.systemId)
  arguments = args
  if args.topology == None and args.help:
    printHelp()
  if args.build:
    build()
  if args.topology != None:
    readTopology(args.topology)
    args.situations = [i for s in args.situations for i in s]
    if args.dse is not None:
      readDSE(args.dse)
    for sit in args.situations:
      readSituation(sit)
    for instance in instances.values():
      instance.applySituation()
      instance.collectArguments()
    if args.nrOfSystems > 1 or args.distributionConfig is not None:
      if not configureDistribution():
        cPrint("Failed to distribute simulations!")
        return
    if args.help:
      printHelp()
    if args.display:
      display()
    if args.execute and args.dse is None:
      run()
    elif args.execute:
      runDSE()
  if not args.execute and not args.display and not args.build and not args.help:
    from appJar import gui
    GUI = gui(handleArgs=False)
    showGui()

def showGui():
  global GUI
  GUI.setGeometry(1280, 720)
  GUI.setFont(11, "Courier")
  
  GUI.addMenuList("File", ["Load Topology"], menuPress)
  GUI.addMenuList("Display", ["Clear Console"], menuPress)
  GUI.addMenuCheckBox("Display", "Verbose", menuPress)
  GUI.addMenuList("Simulation", ["Build", "Stop", "Start"], menuPress)
  GUI.addMenuList("Help", ["Current Config", "Show Help"], menuPress)
  
  GUI.addToolbar(["OPEN", "ABOUT", "HELP", "CLOSE", "FACTORY", "MD-STOP", "MD-PLAY"], menuPress, findIcon=True)
  
  GUI.startPanedFrame("optionsPane")
  GUI.startTabbedFrame("optionsTabs")
  GUI.startTab("Options")
  GUI.addLabel("hdOptions", "Options")
  GUI.stopTab()
  
  GUI.startTab("Instances")
  GUI.stopTab()
  GUI.stopTabbedFrame()
  
  GUI.startPanedFrame("outputPane")
  GUI.addScrolledTextArea("console")
  GUI.disableTextArea("console")
  GUI.stopPanedFrame()
  
  GUI.stopPanedFrame()
  
  GUI.addStatusbar(fields=1)
  status()
  GUI.setTabbedFrameTabExpand("optionsTabs", expand=True)
  GUI.setToolbarButtonDisabled("MD-PLAY")
  GUI.setToolbarButtonDisabled("MD-STOP")
  
  updateGui()
  GUI.go()

def menuPress(menu):
  global GUI, arguments, runSem, running, building, instancesInitialised, stop
  if menu == "Load Topology" or menu == "OPEN":
    arguments.topology = GUI.openBox(title="Load Topology", dirName="conf", fileTypes=[("topologies", "*.topo")])
    readTopology(arguments.topology)
    updateGui()
  
  if menu == "Clear Console" or menu == "CLOSE":
    cPrint(None)
  
  if menu == "Verbose":
    arguments.verbose = not arguments.verbose
    cPrint("Set verbose level " + ("on" if arguments.verbose else "off"))
  
  if menu == "Stop" or menu == "MD-STOP":
    stop = True
    status()
    updateGui()
  
  if menu == "Start" or menu == "MD-PLAY":
    running = True
    stop = False
    status("Running")
    if runSem:
      runSem = False
      GUI.thread(run)
    else:
      runSem = True
    updateGui()
  
  if menu == "Build" or menu == "FACTORY":
    building = True
    status("Building")
    GUI.thread(build)
    updateGui()
  
  if menu == "Current Config" or menu == "ABOUT":
    cPrint(getCurrentConf())
  if menu == "Show Help" or menu == "HELP":
    cPrint(getHelpText())

def updateGui():
  global instancesInitialised
  if GUI is None:
    return False
  if GUI.getMenuCheckBox("Display", "Verbose") != arguments.verbose:
    GUI.setMenuCheckBox("Display", "Verbose")
  if len(instances) > 0 and not instancesInitialised:
    setTitle(federationName)
    GUI.openTab("optionsTabs", "Instances")
    GUI.startTabbedFrame("instanceTabs")
    GUI.setTabbedFrameTabExpand("instanceTabs", expand=True)
    for instance in instances.values():
      GUI.startTab(instance.fedName)
      GUI.setStretch("none")
      for arg in instance.args.items():
        if instance.isLogger and arg[0] == "file":
          continue
        id = instance.fedName + "_" + arg[0]
        GUI.addLabelEntry(id)
        GUI.setEntry(id, str(arg[1]))
        GUI.setEntryChangeFunction(id, instance.updateFromGui)
      for arg in instance.initArgs.items():
        id = instance.fedName + "_Init" + arg[0]
        GUI.addLabelEntry(id)
        GUI.setEntry(id, str(arg[1]))
        GUI.setEntryChangeFunction(id, instance.updateFromGui)
      GUI.stopTab()
    GUI.stopTabbedFrame()
    GUI.stopTab()
    instancesInitialised = True
  updateButtons(["OPEN", "MD-PLAY", "MD-STOP", "FACTORY"])
  updateMenuItems([("File", "Load Topology")])

def updateButtons(btns):
  for btn in btns:
    if getButtonState(btn):
      GUI.setToolbarButtonEnabled(btn)
    else:
      GUI.setToolbarButtonDisabled(btn)

def getButtonState(btn):
  global running, building, instances, runSem
  if btn == "MD-PLAY":
    return len(instances) > 0 and not building and ((not runSem and running) or (runSem and not running))
  if btn == "MD-STOP":
    return len(instances) > 0 and running and not building
  if btn == "FACTORY":
    return not running and not building
  if btn == "OPEN":
    return len(instances) == 0
  return False

def updateMenuItems(items):
  for item in items:
    menu, name = item
    if getMenuItemState(item):
      GUI.enableMenuItem(menu, name)
    else:
      GUI.disableMenuItem(menu, name)

def getMenuItemState(item):
  global running, building, instances
  menu, name = item
  if menu == "File" and name == "Load Topology":
    return len(instances) == 0

def status(msg=None):
  if GUI is None:
    return
  if msg == None:
    msg = "Idle"
  GUI.setStatusbar("Status: " + msg, 0)

def setTitle(title):
  if GUI is not None:
    GUI.setTitle(title)

def build():
  global building
  building = True
  cPrint("Building wrappers...")
  if (not os.path.isdir("build")):
    os.makedirs("build")
  buildCommand = "cd build && "
  if isWindows():
    buildCommand += "cmake -G \"Visual Studio 15 2017 Win64\" -DCMAKE_INSTALL_PREFIX=. " + ("" if arguments.cmake is None else str(arguments.cmake)) + " ../«env.srcDir ?: "src"» && cmake --build . --config Release --target INSTALL"
  else:
    buildCommand += "cmake " + ("" if arguments.cmake is None else str(arguments.cmake)) + " ../«env.srcDir ?: "src"» && make -j4"
  try:
    executeCmd(buildCommand, prepend="", shell=True)
    cPrint("Finished building.")
  except RuntimeError:
    cPrint("Building failed!")
    if GUI is None:
      sys.exit(1)
  building = False
  status()
  updateGui()

def runDSE():
  dseName, _ = os.path.splitext(os.path.basename(arguments.dse))
  cPrint("Running " + str(arguments.dse) + "...")
  setups = None
  setupDict = OrderedDict()
  for key in dse["init"].keys():
    setupDict[key] = [("INIT", key, value) for value in dse["init"][key]]
  for key in dse["conf"].keys():
    setupDict[key] = [("CONF", key, value) for value in dse["conf"][key]]
  if "situ" in dse and dse["situ"] is not None:
    setupDict["SITU"] = [("SITU", value) for value in dse["situ"]]
  if dse["linkedMode"]:
    lengths = [len(variations) for variations in setupDict.values()]
    allEqualLength = all(x == lengths[0] for x in lengths)
    if not allEqualLength:
      cPrint("All DSE argument configuration lists must have equal length!")
      return
    setups = list(zip(*setupDict.values()))
  else:
    setups = list(product(*[keyValue for keyValue in setupDict.values()]))
  cPrint(str(len(setups)) + " DSE configurations to execute.")
  runCounter = 0
  for conf in setups:
    confName, dseParams = setDSEConfiguration(conf, dseName=dseName)
    run(isDSE=True, silent=True)
    if arguments.metrics is not None:
      metricResultLines = readFile(os.path.join(logDir, "MetricResults.txt"))
      metricResults = OrderedDict()
      for line in metricResultLines:
        spl = line.split(": ", 1)
        metricResults[spl[0]] = spl[1]
      dseMetricResults[confName] = { "params": dseParams, "results": metricResults }
    runCounter += 1
    cPrint("Finished " + str(runCounter) + " / " + str(len(setups)) + " configurations (" + confName + ").")
  if arguments.dse is not None:
    combineDSEMetricResults(txtFile=os.path.join("logs", dseName + "_" + startTime, "MetricResults.txt"), csvFile=os.path.join("logs", dseName + "_" + startTime, "MetricResults.csv"))
  cPrint("Finished!")

def setDSEConfiguration(conf, dseName="dse"):
  global instances, logDir, situation
  situation.clear()
  dseParams = []
  readTopology(arguments.topology, silent=True)
  for _, sitName in [s for s in conf if s[0] == "SITU"]:
    readSituation("conf/" + federationName + "/" + sitName + ".situation", silent=True)
    dseParams.append(("Situation", sitName))
  for _, fedName, confName in [s for s in conf if s[0] == "CONF"]:
    readModelConfiguration(confName, fedName, silent=True)
    dseParams.append((fedName + "Configuration", confName))
  for i in instances.values():
    i.applySituation()
  for _, fedAttr, value in [s for s in conf if s[0] == "INIT"]:
    fed, attr = parseKeyValue(fedAttr, splitChar='.')
    instances[fed].initArgs[attr] = value
    dseParams.append((fed + upper(fedAttr), value))
  confName = "_".join([str(tup[-1]) for tup in conf])
  logDir = os.path.join("logs", dseName + "_" + startTime, confName) + "/"
  return confName, dseParams

def run(isDSE=False, silent=False):
  global processes, simulations, loggers, runSem, running, startTimeStamp
  running = True
  if not silent:
    cPrint("Starting...")
  if GUI is not None or arguments.dse is not None:
    updateLogDir()
  os.makedirs(logDir)
  logfile = open(logDir + startTime + ".log", 'w')
  logfile.write(getCurrentConf())
  logfile.close()
  processes.append(executeCmd(rtinodeCommand, blocking=False, shell=True))
  for instance in instances.values():
    if not instance.doStart:
      cPrint("Not starting " + instance.fedName + " because it was manually disabled.")
      continue
    if instance.getAdditionalCommand() != None:
      processes.append(executeCmd(instance.getAdditionalCommand(), blocking=False, shell=True, logFile=logDir + instance.fedName + "_proc.log", env=instance.getAdditionalCommandEnv()))
    if "tmpDir" in instance.args and instance.args["tmpDir"] != None:
      if (not os.path.isdir(instance.args["tmpDir"])):
          os.makedirs(instance.args["tmpDir"])
    ps = executeCmd(instance.getCommand(), blocking=False, logFile=logDir + instance.fedName + ".log")
    processes.append(ps)
    simulations.append(ps)
    if instance.isLogger:
      loggers.append(ps)
    if instance.isMetricProcessor:
      metricProcessors.append(ps)
    time.sleep(0.25 if len(simulations) == 1 else 0.05)
  if not arguments.verbose and not silent:
    cPrint("Processes   : " + str(len(processes)))
    cPrint("Simulations : " + str(len(simulations) - len(loggers) - len(metricProcessors)))
    cPrint("Loggers     : " + str(len(loggers)))
    cPrint("MetricProcs : " + str(len(metricProcessors)))
  try:
    if GUI is None and not isDSE:
      input("Press [ENTER] to start the simulations...\n")
    elif isDSE:
      time.sleep(1.0)
    else:
      cPrint("Press \"Start\" again to start the simulations...")
      while not runSem and not stop:
        time.sleep(0.1)
      if stop:
        exitSimulation(silent=silent)
        return
    for simulation in simulations:
      time.sleep(0.05)
      simulation.stdin.write(os.linesep)
    startTimeStamp = time.time()
    if not silent:
      cPrint("Now running...")
    if len(metricProcessors) > 0:
      while len(metricProcessors) > 0 and not stop:
        for mp in metricProcessors:
          if mp.poll() != None:
            metricProcessors.remove(mp)
        time.sleep(0.25)
      exitSimulation(isDSE=isDSE, silent=silent)
    elif len(loggers) > 0:
      while len(loggers) > 0 and not stop:
        for logger in loggers:
          if logger.poll() != None:
            loggers.remove(logger)
        time.sleep(0.25)
      exitSimulation(isDSE=isDSE, silent=silent)
    else:
      while not stop and all([p.poll() == None for p in processes]):
        time.sleep(0.25)
      exitSimulation(isDSE=isDSE, silent=silent)
  except KeyboardInterrupt:
    exitSimulation(silent=silent)
  except BrokenPipeError:
    cPrint("A process has died unexpectedly!")
    exitSimulation(silent=silent)

def display():
  cPrint(getCurrentConf())

def getCurrentConf():
  return """===== CURRENT CONFIGURATION =====
Topology       : """ + arguments.topology + """
Federation     : """ + federationName + """
Scenario       : """ + str(arguments.scenario) + """
Fault Scenario : """ + str(arguments.faultScenario) + """
Metrics        : """ + str(arguments.metrics) + """
Time           : """ + readableTime + """
Log directory  : """ + logDir + """
FOM file       : """ + arguments.fom + """
RTI address    : """ + arguments.rti + """
Build          : """ + str(arguments.build) + """
Execute        : """ + str(arguments.execute) + """
Render         : """ + str(arguments.render) + """
""" + "\n".join(instance.printConfig() for instance in instances.values()) + """
======= END CONFIGURATION =======""" if arguments.topology is not None else "A topology is required to show the current configuration!"

def readSituation(situationFile, silent=False):
  global situation
  if not silent:
    cPrint("Reading situation " + situationFile + "...")
  f = open(situationFile, "r")
  lines = f.read().splitlines()
  f.close()
  for line in lines:
    k, v = parseKeyValue(line)
    instance, attr = parsePointer(k)
    if instance in situation:
      situation[instance][lower(attr)] = v
    else:
      situation[instance] = { lower(attr) : v }

def readModelConfiguration(configurationFile, instanceName, silent=False):
  global situation
  configurationFile = os.path.join("conf", instances[instanceName].fedClass, configurationFile + ".init")
  if not silent:
    cPrint("Reading modelConfiguration " + configurationFile + "...")
  f = open(configurationFile, "r")
  lines = f.read().splitlines()
  f.close()
  for fedAttr, value in [parseKeyValue(line) for line in lines]:
    _, attr = parsePointer(fedAttr)
    if instanceName in situation:
      situation[instanceName][lower(attr)] = value
    else:
      situation[instanceName] = { lower(attr) : value }
  if not silent:
    cPrint("Finished reading modelConfiguration.")

def readDSE(dseFile):
  global dse, arguments
  cPrint("Reading DSE " + dseFile + "...")
  f = open(dseFile, "r")
  lines = f.read().splitlines()
  f.close()
  _, dseMode = parseKeyValue(lines[0], "mode")
  dse["linkedMode"] = dseMode == "Linked"
  dse["init"] = OrderedDict()
  dse["conf"] = OrderedDict()
  dse["situ"] = None
  for key, value in [parseKeyValue(line) for line in lines[1:]]:
    if key == "scenario":
      arguments.scenario = os.path.join("conf", federationName, value) + ".scenario"
    elif key == "faultScenario":
      arguments.faultScenario = os.path.join("conf", federationName, value) + ".fscenario"
    elif key == "INIT":
      attrRef, values = parseKeyValue(value)
      dse["init"][attrRef] = ast.literal_eval(values)
    elif key == "SITU":
      dse["situ"] = ast.literal_eval(value)
    elif key == "CONF":
      fed, value = parseKeyValue(value)
      dse["conf"][fed] = ast.literal_eval(value)
  cPrint("Finished reading DSE configuration.")

def readTopology(topologyFile, silent=False):
  global rotalumisPort, federationName, arguments, instances
  instances.clear()
  rotalumisPort = 10001
  if not silent:
    cPrint("Reading topology...")
  topFile = open(topologyFile, "r")
  lines = topFile.read().splitlines()
  topFile.close()
  if len(lines) < 3:
    cPrint("Topology requires at least 3 lines!")
    sys.exit(1)
  _, federationName = parseKeyValue(lines[0], "name")
  if not silent:
    cPrint("  FederationName: " + federationName)
  _, nrOfInstances = parseKeyValue(lines[1], "federates")
  nrOfInstances = int(nrOfInstances)
  if not silent:
    cPrint("      #Federates: " + str(nrOfInstances))
  for i in range(2, 2 + nrOfInstances):
    fedName, tl = parseKeyValue(lines[i])
    fedClass, _ = parseKeyValue(tl)
    disabled = fedName in arguments.disable
    instance = None
    «federateObjects.map[generateReadTopology].join('\n')»
    if instance == None:
      instance = NoneFederateClass(fedName, topologyFile, arguments.rti)
    instances[fedName] = instance
  _, nrOfConnections = parseKeyValue(lines[2 + nrOfInstances], "connections")
  nrOfConnections = int(nrOfConnections)
  if not silent:
    cPrint("    #Connections: " + str(nrOfConnections))
  for i in range(3 + nrOfInstances, 3 + nrOfInstances + nrOfConnections):
    (destFed, destAttr), (srcFed, srcAttr) = parseConnection(lines[i])
    connections.append(Connection(destFed, destAttr, srcFed, srcAttr))
  if arguments.metrics is not None:
    addMetricProcessor()
  if not silent:
    cPrint("Finished reading topology.")

def configureDistribution(silent=False):
  global arguments, instances, rtinodeCommand
  if not silent:
    cPrint("Configuring distributed simulation...")
  if arguments.systemId != 0 and arguments.parentRtiHost is None:
    cPrint("Systems not being the parent host should be provided with a Parent RTI Hostname!")
    return False
  
  if arguments.distributionConfig is not None:
    fileContents = readFile(arguments.distributionConfig)
    if len(fileContents) < 1:
      cPrint("Invalid distribution configuration provided.")
      return False
    _, nrOfSystems = parseKeyValue(fileContents[0], "#systems")
    nrOfSystems = int(nrOfSystems)
    if arguments.systemId >= nrOfSystems or arguments.systemId < 0:
      cPrint("Invalid SystemID!")
      return False
    distribution = {key: OrderedDict() for key in range(nrOfSystems)}
    for line in fileContents[1:]:
      fName, system = parseKeyValue(line)
      distribution[int(system)][fName] = instances[fName]
    instances = distribution[arguments.systemId]
  else:
    if arguments.systemId >= arguments.nrOfSystems or arguments.systemId < 0:
      cPrint("Invalid SystemID!")
      return False
    if arguments.nrOfSystems > len(instances):
      cPrint("Pointless distribution: more systems than instances!")
      return False
    distribution = Distribution(arguments.nrOfSystems)
    distribution.addAll(instances)
    instances = distribution.getInstances(arguments.systemId)
  
  if arguments.systemId == 0:
    rtinodeCommand = "rtinode -i '*:" + str(arguments.parentRtiPort) + "'"
    for instance in instances.values():
      instance.rtiHostname = "localhost:" + str(arguments.parentRtiPort)
  else:
    rtinodeCommand = "rtinode -p '" + arguments.parentRtiHost + ":" + str(arguments.parentRtiPort) + "'"
  return True


def addMetricProcessor():
  global instances, arguments
  if arguments.metrics is None:
    return
  metFile = open(arguments.metrics, "r")
  lines = metFile.read().splitlines()
  metFile.close()
  _, measureTime = parseKeyValue(lines[1], "measuretime")
  instances["MetricProcessor"] = MetricProcessor(arguments.topology, arguments.rti, arguments.metrics, measureTime=float(measureTime))

def combineDSEMetricResults(txtFile=None, csvFile=None):
  if txtFile is not None:
    f = open(txtFile, 'w')
    for configuration, contents in dseMetricResults.items():
      f.write((" CONFIGURATION " + configuration + " ").center(50, '=') + os.linesep)
      f.write("========" + ("PARAMETERS").center(34, ' ') + "========" + os.linesep)
      for paramKey, paramValue in contents["params"]:
        f.write(str(paramKey).ljust(38) + ": " + str(paramValue) + os.linesep)
      f.write("========" + ("RESULTS").center(34, ' ') + "========" + os.linesep)
      for metricName, metricResult in contents["results"].items():
        f.write(str(metricName).ljust(38) + ": " + str(metricResult) + os.linesep)
      f.write("".center(50, '='))
      f.write(os.linesep + os.linesep)
    f.close()
    cPrint("Written metric results to " + txtFile + "...")
  if csvFile is not None:
    f = open(csvFile, 'w')
    headersPrinted = False
    for configuration, contents in dseMetricResults.items():
      if not headersPrinted:
        isFirst = True
        for paramKey, paramValue in contents["params"]:
          if not isFirst:
            f.write(";")
          else:
            isFirst = False
          f.write(str(paramKey))
        if len(contents["results"]) > 0:
          for metricName in contents["results"].keys():
            f.write(";" + str(metricName))
        f.write(os.linesep)
        headersPrinted = True
      isFirst = True
      for paramKey, paramValue in contents["params"]:
        if not isFirst:
          f.write(";")
        else:
          isFirst = False
        f.write(str(paramValue))
      for _, metricResult in contents["results"].items():
        if not isFirst:
          f.write(";")
        else:
          isFirst = False
        f.write(str(metricResult))
      f.write(os.linesep)
    f.close()

def parseKeyValue(line, expectedKey=None, splitChar=':', num=1):
  spl = line.split(splitChar, num)
  if len(spl) == 2 and (expectedKey == None or spl[0] == expectedKey):
    return spl[0], spl[1]
  cPrint("Failed to parse " + expectedKey + "!")
  sys.exit(1)

def parseConnection(line):
  spl = line.split('-')
  if len(spl) != 2:
    cPrint("Invalid connection definition:",line)
    sys.exit(1)
  return parsePointer(spl[0]), parsePointer(spl[1])

def parsePointer(s):
  spl = s.split('.')
  if len(spl) != 2:
    cPrint("Invalid pointer definition:",s)
    sys.exit(1)
  return spl[0], spl[1]

def printHelp():
  cPrint(getHelpText())
  sys.exit(0)

def getHelpText():
  if arguments.topology != None:
    return getGenericHelpText() + """
""" + getInstanceHelpText()
  else:
    return getGenericHelpText()

def getGenericHelpText():
  return """=== GENERIC FLAGS ===
  -b, --build   : If set, the project will be built.
  -e, --execute : If set, the given topology will be executed.
  -d, --display : If set, the current configuration will be displayed.
  -h, --help    : If set, this help message will be displayed.
  --render      : If set, all 3D rendering simulators will display their renders.
  -v, --verbose : If set, all external command outputs will be displayed.

=== GENERIC VARIABLES ===
  -t X, --topology X    : Sets the topology file to X.
  -f X, --fom X         : Sets the FOM XML to file X.
  -r X, --rti X         : Sets the RTI address to X.
  -s X+, --situation X+ : Sets the given situation(s) X.
  -u X, --scenario X    : Sets the given user scenario X.
  -m X, --metrics X     : Sets the given metrics file X.
  --faults X            : Sets the given fault scenario X.
  --dse X               : Specifies the Design Space Exploration execution to X.
  --disable X           : Disable federate X to start.
  --cmake=X             : Specifies additional CMake flags X for building.

=== DISTRIBUTION VARIABLES ===
  --distribution X      : Specifies a predefined federate distribution X.
  --systemId X          : Sets the system ID of this system to X (default 0).
  --nrOfSystems X       : Sets the number of systems for the distribution to X.
  --parentHost X        : Sets the parent RTI hostname to X.
  --parentPort X        : Sets the parent RTI port to X."""

def getInstanceHelpText():
  return """
""" + "\n\n".join(map(lambda i: """=== INSTANCE """ + i.fedName + """ ===
""" + "\n".join(map(lambda h: """--""" + ("\033[1m\033[36m" if not isWindows() and GUI is None else "") + (lower(i.fedName) + upper(h[0]) + ("\033[0m" if not isWindows() and GUI is None else "") + " X").ljust(26) + ": " + h[1][0] + " (default: " + str(h[1][1]) + ")", i.help.items())) + ("" if len(i.initHelp) == 0 else """
""" + "\n".join(map(lambda h: """--""" + ("\033[1m\033[36m" if not isWindows() and GUI is None else "") + (lower(i.fedName) + "Init" + upper(h[0]) + ("\033[0m" if not isWindows() and GUI is None else "") + " X").ljust(26) + ": " + h[1][0] + " (default: " + str(h[1][1]) + ")", i.initHelp.items()))), instances.values())) + """
"""

def isWindows():
  return os.name == 'nt'

def executeCmd(command, blocking=True, shell=not isWindows(), prepend="" if isWindows() else "exec ", logFile=None, env=None):
  global arguments
  nonVerboseOut = open(logFile, "wb") if logFile is not None else subprocess.DEVNULL
  process = subprocess.Popen(prepend + command, bufsize=1, stdin=subprocess.PIPE, stdout=(None if arguments.verbose else nonVerboseOut), stderr=subprocess.STDOUT, shell=shell, universal_newlines=True, env=env)
  if blocking:
    if process.wait() != 0:
      raise RuntimeError
    return None
  else:
    return process

def readFile(file):
  if not os.path.isfile(file):
    return []
  f = open(file, "r")
  lines = f.read().splitlines()
  f.close()
  return lines

def lower(s):
  if len(s) == 0:
    return s
  return s[0].lower() + s[1:]

def upper(s):
  if len(s) == 0:
    return s
  return s[0].upper() + s[1:]

def updateTime():
  global readableTime, startTime
  readableTime = time.strftime("%m/%d/%Y %H:%M:%S")
  startTime = time.strftime("%Y%m%d%H%M%S")

def updateLogDir():
  global logDir
  if GUI is not None and arguments.dse is None:
    updateTime()
    logDir = "logs/" + startTime + "/"
  for instance in instances.values():
    if instance.isLogger:
      instance.args["file"] = logDir + instance.fedName + ".csv"
    if instance.isMetricProcessor:
      instance.args["outputFile"] = logDir + "MetricResults.txt"

def exitSimulation(isDSE=False, silent=False):
  global processes, simulations, loggers, running, runSem
  runDuration = round(time.time() - startTimeStamp, 2)
  if not silent:
    cPrint("Now exiting...")
  for mp in metricProcessors:
    mp.terminate()
  for logger in loggers:
    logger.terminate()
  time.sleep(1.0)
  for simulator in simulations:
    simulator.kill()
  for process in processes:
    process.kill()
  if not silent:
    cPrint("Done!")
    cPrint("Simulation time: " + str(runDuration) + " s")
  processes.clear()
  simulations.clear()
  loggers.clear()
  running = False
  runSem = True
  if isDSE:
    return
  elif GUI is None:
    sys.exit(0)
  else:
    updateGui()

def cPrint(str):
  global GUI
  if GUI == None:
    print(str)
  else:
    GUI.enableTextArea("console")
    if str is None:
      GUI.clearTextArea("console")
    else:
      GUI.setTextArea("console", str + os.linesep, callFunction=False)
    GUI.disableTextArea("console")

if __name__ == "__main__":
  main()
'''

  static def generateFederateClass(FederateObject fed)
'''«val args = Utils.getSimulatorTypeArguments(fed) as List<ClassArgument>»
«val initArgs = Utils.getInitialisableArguments(fed)»
«val rotalumis = fed.type == FederateType.POOSL»
«val csv = fed.type == FederateType.CSV»
class «fed.name.toFirstUpper»FederateClass(FederateClass):
  def __init__(self, fedName, topology, rtiHostname«FOR arg : args», «arg.name.toFirstLower»=«IF arg.defaultValue !== null»«arg.defaultValueAsString»«ELSE»None«ENDIF»«ENDFOR»«FOR arg : initArgs», «arg.name.toFirstLower»«IF arg.defaultValue !== null»=«arg.defaultValueAsString»«ENDIF»«ENDFOR», additionalCommand=None, doStart=True):
    super().__init__(fedName, "«fed.name.toFirstUpper»", topology, rtiHostname, additionalCommand«IF rotalumis»=rotalumisExecutable + " -p " + str(port), additionalCommandEnv={'PORT': str(port), 'LC_ALL': 'C'}«ENDIF»«IF csv», isLogger=True«ENDIF», doStart=doStart«IF fed.simulationWeight > 0», simWeight=«fed.simulationWeight»«ENDIF»)
    «IF !args.empty»
    self.args = { «args.map[a | "\"" + a.name.toFirstLower + "\": " + a.name.toFirstLower].join(", ")» }
    self.help = { «args.map[a | "\"" + a.name.toFirstLower + "\": (\"" + a.helpText + ".\", \"" + a.defaultValue + "\")"].join(", ")» }
    self.argKeyOrder = [ «args.filter[a | !a.excludeFromCmd].map[a | "\"" + a.name.toFirstLower + "\""].join(", ")» ]
    «ENDIF»«IF !initArgs.empty»
    self.initArgs = { «initArgs.map[a | "\"" + a.name.toFirstLower + "\": " + a.name.toFirstLower].join(", ")» }
    self.initHelp = { «initArgs.map[a | "\"" + a.name.toFirstLower + "\": (\"" + a.helpText + ".\", \"" + a.defaultValue + "\")"].join(", ")» }
    self.initArgKeyOrder = [ «initArgs.filter[a | !a.excludeFromCmd].map[a | "\"" + a.name.toFirstLower + "\""].join(", ")» ]
    «ENDIF»
  
  «IF fed.type == FederateType.POOSL»
  def setRotalumisPort(self, rotalumisPort):
  	self.args["port"] = rotalumisPort

  def getAdditionalCommand(self):
    return rotalumisExecutable + " -p " + str(self.args["port"])
  
  «ENDIF»
  def updateFromGui(self, key):
    if GUI is None:
      return False
    if key.find(self.fedName + "_Init") >= 0:
      self.initArgs[key.replace(self.fedName + "_Init", "")] = GUI.getEntry(key)
    elif key.find(self.fedName + "_") >= 0:
      self.args[key.replace(self.fedName + "_", "")] = GUI.getEntry(key)'''

  static def generateReadTopology(FederateObject fedObj)
'''if fedClass == "«fedObj.name.toFirstUpper»":
  «val rotalumis = fedObj.type == FederateType.POOSL»
  «val fmu = fedObj.type == FederateType.FMU»
  «val collision = fedObj.type == FederateType.COLSIM»
  instance = «fedObj.name.toFirstUpper»FederateClass(fedName, topologyFile, arguments.rti«IF fmu», tmpDir="/tmp/" + fedName«ELSEIF collision», render=arguments.render«ENDIF», doStart=not disabled«IF rotalumis», port=rotalumisPort«ENDIF»)«IF rotalumis»
  rotalumisPort += 1«ENDIF»'''
  
}