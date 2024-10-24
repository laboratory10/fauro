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
#define RESET_PIN (12)

namespace Components {

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
    FatalReceive_handler(
        NATIVE_INT_TYPE portNum,
        FwEventIdType Id
    )
  {
    // for **nix, delay then exit with error code
    Os::Log::logMsg("FATAL %d handled.\n",Id,0,0,0,0,0);
    digitalWrite(RESET_PIN, HIGH);
    pinMode(RESET_PIN, OUTPUT);
    Os::Log::logMsg("Goodnight.\n",0,0,0,0,0,0);
    delay(100);
    digitalWrite(RESET_PIN, LOW);
    while (true) {} // Returning might be bad
  }

}
