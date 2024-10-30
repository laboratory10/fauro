// ======================================================================
// \title  PowerManager.cpp
// \author laboratory10
// \brief  cpp file for PowerManager component implementation class
// ======================================================================

#include "Components/PowerManager/PowerManager.hpp"
#include "FpConfig.hpp"
#include <variant.h>
#include <Arduino.h>

namespace Components {

  #define VBATPIN (A7)

  // ----------------------------------------------------------------------
  // Component construction and destruction
  // ----------------------------------------------------------------------

  PowerManager ::
    PowerManager(const char* const compName) :
      PowerManagerComponentBase(compName)
  {

  }

  PowerManager ::
    ~PowerManager()
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void PowerManager ::
    schedIn_handler(
        NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    //VBATPIN*2 cells*3.3V reference/1024 to convert to Volts = battery voltage
    this->tlmWrite_BAT_VOLTAGE((float) (analogRead(VBATPIN)*2*3.3/1024));
  }

}
