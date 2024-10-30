// ======================================================================
// \title  FatalHandler.cpp
// \author laboratory10
// \brief  cpp file for FatalHandler component implementation class
// ======================================================================

#include <cstdlib>
#include <Os/Log.hpp>
#include "Components/FauroFatalHandler/FauroFatalHandler.hpp"
#include "FpConfig.hpp"
#include <Arduino.h>
#include <delay.h>

namespace Components {

  #define RESET_PIN (12)
  bool response_triggered = false;

  // ----------------------------------------------------------------------
  // Component construction and destruction
  // ----------------------------------------------------------------------

  FauroFatalHandler ::
    FauroFatalHandler(const char* const compName) :
      FauroFatalHandlerComponentBase(compName)
  {

  }

  FauroFatalHandler ::
    ~FauroFatalHandler()
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void FauroFatalHandler ::
    fatalCheck_handler(
        NATIVE_INT_TYPE portNum
    )
  {
    if (response_triggered) {
      Os::Log::logMsg("FATAL handled.\n");
      digitalWrite(RESET_PIN, HIGH);
      pinMode(RESET_PIN, OUTPUT);
      Os::Log::logMsg("Goodnight.\n");
      delay(100);
      digitalWrite(RESET_PIN, LOW);
      while (true) {} // prevent execution from continuing if the reset fails
    }
  }

  void FauroFatalHandler ::
    fatalReceive_handler(
        NATIVE_INT_TYPE portNum,
        FwEventIdType Id
    )
  {
    response_triggered = true;
  }

}
