// ======================================================================
// \title  TelemetryManager.cpp
// \author snow
// \brief  cpp file for TelemetryManager component implementation class
// ======================================================================

#include "Components/TelemetryManager/TelemetryManager.hpp"
#include "FpConfig.hpp"

#include <Arduino.h>
#include <RH_RF69.h>
extern RH_RF69 rf69;
uint32_t last_groundspeak = 0;

namespace Components {

  // ----------------------------------------------------------------------
  // Component construction and destruction
  // ----------------------------------------------------------------------

  TelemetryManager ::
    TelemetryManager(const char* const compName) :
      TelemetryManagerComponentBase(compName)
  {

  }

  TelemetryManager ::
    ~TelemetryManager()
  {

  }


  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void TelemetryManager :: schedIn_handler(NATIVE_INT_TYPE portNum, NATIVE_UINT_TYPE context) {
    Fw::Buffer recv_buffer = this->allocate_out(0, SERIAL_BUFFER_SIZE);
    read_data(recv_buffer);
    if (millis() - last_groundspeak > 500) {
        uint8_t message[60];
        uint8_t packet_size = 60;
        memset(message, '\0', 60);
        message[0] = '-';
        rf69.send(message, 60);
        memset(message, '\0', 60);
        rf69.waitAvailableTimeout(60);
        if (rf69.available()) {
            if (rf69.recv(message, &packet_size)) {
                memcpy(recv_buffer.getData(), message, packet_size);
                recv_buffer.setSize(packet_size);
                this->tlmWrite_LAST_RSSI(rf69.lastRssi());
            }
        }
        last_groundspeak = millis();
    }
    recv_out(0, recv_buffer, Drv::RecvStatus::RECV_OK);
  }

  Drv::SendStatus TelemetryManager :: send_handler(NATIVE_INT_TYPE portNum, Fw::Buffer& sendBuffer) {
    write_data(sendBuffer);
    uint8_t radio_msg[180];
    memset(radio_msg, '\0', 180);
    memcpy(radio_msg, sendBuffer.getData(), sendBuffer.getSize());
    //why are these delays necessary? ugh idk...best guess is the other radio is too slow printing to serial
    if (sendBuffer.getSize() <= 60) {
        rf69.send(radio_msg, sendBuffer.getSize());
        delay(5);
    } else if (sendBuffer.getSize() <= 120) {
        rf69.send(radio_msg, 60);
        delay(5);
        rf69.send(radio_msg+60, 60);
        delay(5);
    } else {
        rf69.send(radio_msg, 60);
        delay(5);
        rf69.send(radio_msg+60, 60);
        delay(5);
        rf69.send(radio_msg+120, 60);
        delay(5);
    }

    deallocate_out(0, sendBuffer);
    return Drv::SendStatus::SEND_OK;
  }

}
