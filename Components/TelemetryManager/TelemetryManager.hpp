// ======================================================================
// \title  TelemetryManager.hpp
// \author snow
// \brief  hpp file for TelemetryManager component implementation class
// ======================================================================

#ifndef Components_TelemetryManager_HPP
#define Components_TelemetryManager_HPP

#include "Components/TelemetryManager/TelemetryManagerComponentAc.hpp"
#include <FprimeArduino.hpp>

namespace Components {

  class TelemetryManager :
    public TelemetryManagerComponentBase
  {

    public:
      const static FwSizeType SERIAL_BUFFER_SIZE = 64;  // Maximum Arduino buffer size
      // ----------------------------------------------------------------------
      // Component construction and destruction
      // ----------------------------------------------------------------------

      //! Construct TelemetryManager object
      TelemetryManager(
          const char* const compName //!< The component name
      );

      //! Destroy TelemetryManager object
      ~TelemetryManager();

      //! Configure this port
      void configure(Stream* streamDriver);

    PRIVATE:
      //! Read the actual data
      void read_data(Fw::Buffer& fwBuffer);
      //! Write the actual data
      void write_data(Fw::Buffer& fwBuffer);
      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for schedIn
      //!
      //! Polling for receiving data
      void schedIn_handler(
          NATIVE_INT_TYPE portNum, //!< The port number
          NATIVE_UINT_TYPE context //!< The call order
      ) override;

      //! Handler implementation for send
      //!
      //! Takes data to transmit out the UART device
      Drv::SendStatus send_handler(
          NATIVE_INT_TYPE portNum, //!< The port number
          Fw::Buffer& sendBuffer //!< Data to send
      ) override;

      //! Port number to open
      NATIVE_UINT_TYPE m_port_number;
      //! Stores the open stream port, POINTER_CAST so Linux and Ardunio may use different types
      void* m_port_pointer;

  };

}

#endif
