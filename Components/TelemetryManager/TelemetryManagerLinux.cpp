// ======================================================================
// \title  TelemetryManagerLinux.cpp
// \author laboratory10
// \brief  cpp file for TelemetryManager component implementation class
// ======================================================================

#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <Components/TelemetryManager/TelemetryManager.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace Components {
  #define SERIAL_FILE_LINUX_TMPL "/dev/pts/%d"
  
  char** SERIAL_PORT = NULL;
  void TelemetryManager ::init(
    const FwIndexType port_number, 
    const PlatformIntType baud) 
  {
    char name[1024];
    TelemetryManagerComponentBase::init(instance);
    // NULL ports use above template
    if (SERIAL_PORT == NULL) {
      snprintf(name, 1024, SERIAL_FILE_LINUX_TMPL, m_port_number + 20);
      SERIAL_PORT = reinterpret_cast<char**>(&name);
    }
    printf("Opening serial port: '%s'\n", *SERIAL_PORT);
    m_port_pointer = open(*SERIAL_PORT, O_RDWR);
    int flags = fcntl(m_port_pointer, F_GETFL, 0);
    fcntl(m_port_pointer, F_SETFL, flags | O_NONBLOCK);
  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void TelemetryManager ::write_data(Fw::Buffer& fwBuffer) {
    if (m_port_pointer != -1) {
      write(
        m_port_pointer, 
        reinterpret_cast<U8*>(fwBuffer.getdata()), 
        fwBuffer.getsize());
    }
  }

  void TelemetryManager ::read_data(Fw::Buffer& fwBuffer) {
      NATIVE_INT_TYPE result;
      if ((m_port_pointer != -1) &&
          (-1 != (result = read(
            m_port_pointer,
            reinterpret_cast<U8*>(fwBuffer.getdata()),
            fwBuffer.getsize())))) 
      {
          fwBuffer.setsize(result);
      } else {
          fwBuffer.setsize(0);
      }
  }
}  // namespace Components
