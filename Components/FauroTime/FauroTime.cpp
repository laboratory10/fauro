// ======================================================================
// \title  FauroTime.cpp
// \author snow
// \brief  cpp file for FauroTime component implementation class
// ======================================================================

#include "Components/FauroTime/FauroTime.hpp"
#include "FpConfig.hpp"
#include <TimeLib.h>
#include <Arduino.h>

namespace Components {

  // ----------------------------------------------------------------------
  // Component construction and destruction
  // ----------------------------------------------------------------------

  FauroTime ::
    FauroTime(const char* const compName) :
      FauroTimeComponentBase(compName)
  {

  }

  FauroTime ::
    ~FauroTime()
  {

  }

  void FauroTime::timeGetPort_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::Time &time /*!< The time to set */
    ) {
        time_t sec = now();
        U32 usec = (millis() % 1000)*1000;
        time.set(sec, usec);
    }

  // ----------------------------------------------------------------------
  // Handler implementations for commands
  // ----------------------------------------------------------------------

  void FauroTime ::
    TIME_SET_cmdHandler(
        FwOpcodeType opCode,
        U32 cmdSeq,
        U32 epoch_offset_seconds
    )
  {
    setTime(epoch_offset_seconds);
    this->log_ACTIVITY_HI_TIME_FROM_EPOCH_SET();
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

}
