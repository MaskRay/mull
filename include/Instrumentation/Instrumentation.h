#pragma once

#include "Instrumentation/Callbacks.h"
#include "Instrumentation/DynamicCallTree.h"
#include "Testee.h"

#include <vector>

namespace llvm {
  class Module;
}

namespace mull {
  class Driver;
  class Test;
  class Filter;

  class Instrumentation {
  public:
    Instrumentation();

    void recordFunctions(llvm::Module *originalModule);
    void insertCallbacks(llvm::Module *instrumentedModule);

    std::vector<std::unique_ptr<Testee>> getTestees(Test *test, Filter &filter, int distance);

    void setupInstrumentationInfo(Test *test);
    void cleanupInstrumentationInfo(Test *test);

    std::map<std::string, uint64_t> &getFunctionOffsetMapping();

    const char *instrumentationInfoVariableName();
    const char *functionIndexOffsetPrefix();
  private:
    Callbacks callbacks;
    std::vector<CallTreeFunction> functions;
    std::map<std::string, uint64_t> functionOffsetMapping;
  };
}
