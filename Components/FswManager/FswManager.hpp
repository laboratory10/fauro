// ======================================================================
// \title  FswManager.hpp
// \author snow
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
      // Handler implementations for commands
      // ----------------------------------------------------------------------

      //! Handler implementation for command FSW_IMAGE_CRC
      void FSW_IMAGE_CRC_cmdHandler(
          FwOpcodeType opCode, //!< The opcode
          U32 cmdSeq //!< The command sequence number
      ) override;

  };

}

#endif
