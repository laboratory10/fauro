// ======================================================================
// \title  FatalHandler.hpp
// \author snow
// \brief  hpp file for FatalHandler component implementation class
// ======================================================================

#ifndef Components_FauroFatalHandler_HPP
#define Components_FauroFatalHandler_HPP

#include "Components/FauroFatalHandler/FauroFatalHandlerComponentAc.hpp"

namespace Components {

  class FauroFatalHandler :
    public FauroFatalHandlerComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Component construction and destruction
      // ----------------------------------------------------------------------

      //! Construct FauroFatalHandler object
      FauroFatalHandler(
          const char* const compName //!< The component name
      );

      //! Destroy FauroFatalHandler object
      ~FauroFatalHandler();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for FatalReceive
      //!
      //! FATAL event receive port
      void FatalReceive_handler(
          NATIVE_INT_TYPE portNum, //!< The port number
          FwEventIdType Id //!< The ID of the FATAL event
      ) override;

  };

}

#endif
