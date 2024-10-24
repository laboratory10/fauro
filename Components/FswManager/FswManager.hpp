// ======================================================================
// \title  FswManager.hpp
// \author laboratory10
// \brief  hpp file for FswManager component implementation class
// ======================================================================

#ifndef Components_FswManager_HPP
#define Components_FswManager_HPP

#include "Components/FswManager/FswManagerComponentAc.hpp"

namespace Components {

  class FswManager :
    public FswManagerComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Component construction and destruction
      // ----------------------------------------------------------------------

      //! Construct FswManager object
      FswManager(
          const char* const compName //!< The component name
      );

      //! Destroy FswManager object
      ~FswManager();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------
      
      //! Handler implementation for modeRequest
      void modeRequest_handler(
          NATIVE_INT_TYPE portNum, //!< The port number
          Types::SYS_MODE& mode, //!< a place to put the current system mode
          Fw::ParamValid& valid //!< a place to indicate whether the parameter value is valid
      ) override;

      void schedIn_handler(
          NATIVE_INT_TYPE portNum, //!< The port number
          NATIVE_UINT_TYPE context //!< The call order
      ) override;

      // ----------------------------------------------------------------------
      // Handler implementations for commands
      // ----------------------------------------------------------------------

      //! Handler implementation for command FSW_IMAGE_CRC
      void FSW_IMAGE_CRC_cmdHandler(
          FwOpcodeType opCode, //!< The opcode
          U32 cmdSeq //!< The command sequence number
      ) override;

      //! Handler implementation for command FSW_RESET
      void FSW_RESET_cmdHandler(
          FwOpcodeType opCode, //!< The opcode
          U32 cmdSeq //!< The command sequence number
      ) override;

      void parameterUpdated(FwPrmIdType id /*!< The parameter ID*/
    ) override;

  };

}

#endif
