// ======================================================================
// \title  GncManager.hpp
// \author snow
// \brief  hpp file for GncManager component implementation class
// ======================================================================

#ifndef Components_GncManager_HPP
#define Components_GncManager_HPP

#include "Components/GncManager/GncManagerComponentAc.hpp"

namespace Components {

  class GncManager :
    public GncManagerComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Component construction and destruction
      // ----------------------------------------------------------------------

      //! Construct GncManager object
      GncManager(
          const char* const compName //!< The component name
      );

      //! Destroy GncManager object
      ~GncManager();

      // Initial setup of the GNC sensors
      void initialize();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for gncUpdate
      void gncUpdate_handler(
          NATIVE_INT_TYPE portNum, //!< The port number
          NATIVE_UINT_TYPE context //!< The call order
      ) override;

  };

}

#endif
