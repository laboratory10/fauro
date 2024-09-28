// ======================================================================
// \title  PowerManager.hpp
// \author snow
// \brief  hpp file for PowerManager component implementation class
// ======================================================================

#ifndef Components_PowerManager_HPP
#define Components_PowerManager_HPP

#include "Components/PowerManager/PowerManagerComponentAc.hpp"

namespace Components {

  class PowerManager :
    public PowerManagerComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Component construction and destruction
      // ----------------------------------------------------------------------

      //! Construct PowerManager object
      PowerManager(
          const char* const compName //!< The component name
      );

      //! Destroy PowerManager object
      ~PowerManager();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for schedIn
      void schedIn_handler(
          NATIVE_INT_TYPE portNum, //!< The port number
          NATIVE_UINT_TYPE context //!< The call order
      ) override;

  };

}

#endif
