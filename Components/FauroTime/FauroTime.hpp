// ======================================================================
// \title  FauroTime.hpp
// \author snow
// \brief  hpp file for FauroTime component implementation class
// ======================================================================

#ifndef Components_FauroTime_HPP
#define Components_FauroTime_HPP

#include "Components/FauroTime/FauroTimeComponentAc.hpp"

namespace Components {

  class FauroTime :
    public FauroTimeComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Component construction and destruction
      // ----------------------------------------------------------------------

      //! Construct FauroTime object
      FauroTime(
          const char* const compName //!< The component name
      );

      //! Destroy FauroTime object
      ~FauroTime();

      protected:
        void timeGetPort_handler(
                NATIVE_INT_TYPE portNum, /*!< The port number*/
                Fw::Time &time /*!< The U32 cmd argument*/
            );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for commands
      // ----------------------------------------------------------------------

      //! Handler implementation for command TIME_SET
      void TIME_SET_cmdHandler(
          FwOpcodeType opCode, //!< The opcode
          U32 cmdSeq, //!< The command sequence number
          U32 epoch_offset_seconds
      ) override;

  };

}

#endif
