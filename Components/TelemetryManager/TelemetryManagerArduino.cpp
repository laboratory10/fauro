// ======================================================================
// \title  TelemetryManagerArduino.cpp
// \author laboratory10
// \brief  cpp file for TelemetryManager component implementation class
// ======================================================================

#include <Components/TelemetryManager/TelemetryManager.hpp>
#include <FprimeArduino.hpp>
#include <Os/Log.hpp>
#include "Fw/Types/BasicTypes.hpp"
#include "Fw/Types/Assert.hpp"

#include <RH_RF69.h>
#define RADIO_CS    (6)
#define RADIO_INT   (5)
#define RADIO_RST   (4)
#define LOW             (0x0)
#define HIGH            (0x1)
#define INPUT           (0x0)
#define OUTPUT          (0x1)
RH_RF69 rf69(RADIO_CS, RADIO_INT);

namespace Components {

void TelemetryManager::configure(Stream* streamDriver) {
    //Force radio reset
    pinMode(RADIO_RST, OUTPUT);
    digitalWrite(RADIO_RST, LOW);
    digitalWrite(RADIO_RST, HIGH);
    delay(10);
    digitalWrite(RADIO_RST, LOW);
    delay(10);

    rf69.init();
    rf69.setFrequency(915.0);//radio_freq_success = radio.setFrequency(RADIO_FREQ);
    rf69.setTxPower(20, true);
    uint8_t key[] = {0xd6, 0xa9, 0x1b, 0x42, 0x63, 0x21, 0x52, 0x6d,
                    0x27, 0xbc, 0x04, 0xff, 0xc4, 0xa9, 0x8c, 0xf0};
    rf69.setEncryptionKey(key);

    FW_ASSERT(streamDriver != nullptr);
    m_port_pointer = streamDriver;
    if (this->isConnected_ready_OutputPort(0)) {
        this->ready_out(0);
    }
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void TelemetryManager ::write_data(Fw::Buffer& fwBuffer) {
    FW_ASSERT(m_port_pointer != 0);
    reinterpret_cast<Stream*>(m_port_pointer)
        ->write(reinterpret_cast<U8*>(fwBuffer.getData()), fwBuffer.getSize());
}

void TelemetryManager ::read_data(Fw::Buffer& fwBuffer) {
    Stream* stream_ptr = reinterpret_cast<Stream*>(m_port_pointer);
    int byte = 0;
    NATIVE_UINT_TYPE count = 0;
    U8* raw_data = reinterpret_cast<U8*>(fwBuffer.getData());
    while ((stream_ptr->available() > 0) && (count < fwBuffer.getSize()) && ((byte = stream_ptr->read()) != -1)) {
        *(raw_data + count) = static_cast<U8>(byte);
        count++;
    }
    fwBuffer.setSize(count);
}
}  // namespace Components
