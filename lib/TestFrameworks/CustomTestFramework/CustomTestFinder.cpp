#include "mull/TestFrameworks/CustomTestFramework/CustomTestFinder.h"

#include "mull/Config/Configuration.h"
#include "mull/Filter.h"
#include "mull/Logger.h"
#include "mull/Program/Program.h"
#include "mull/TestFrameworks/Test.h"

#include <llvm/IR/Module.h>

#include <map>
#include <string>
#include <vector>

using namespace mull;
using namespace llvm;

CustomTestFinder::CustomTestFinder(
    const std::vector<CustomTestDefinition> &definitions)
    : testDefinitions(definitions) {}

std::vector<Test> CustomTestFinder::findTests(Program &program,
                                              Filter &filter) {
  std::map<std::string, std::vector<CustomTestDefinition>> testMapping;
  for (const auto &definition : testDefinitions) {
    testMapping[definition.methodName].push_back(definition);
  }

  std::vector<Test> tests;

  for (auto &currentModule : program.modules()) {
    for (auto &function : currentModule->getModule()->getFunctionList()) {
      if (function.isDeclaration()) {
        continue;
      }

      const std::string &functionName = function.getName().str();
      if (testMapping.count(functionName) == 0) {
        continue;
      }

      for (auto &definition : testMapping.at(functionName)) {
        if (filter.shouldSkipTest(definition.testName)) {
          continue;
        }

        std::string programName = definition.programName;
        if (programName.empty()) {
          programName = "mull";
        }

        tests.push_back(Test(definition.testName, programName, "main",
                             definition.callArguments, &function));
      }
    }
  }

  return tests;
}
