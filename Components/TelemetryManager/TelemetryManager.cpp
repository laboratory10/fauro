// ======================================================================
// \title  TelemetryManager.cpp
// \author snow
// \brief  cpp file for TelemetryManager component implementation class
// ======================================================================

#include "Components/TelemetryManager/TelemetryManager.hpp"
#include "FpConfig.hpp"

#include <SD.h>
#include <TimeLib.h>
#include <Components/FswManager/FswManager.hpp>

#include <Arduino.h>
#include <RH_RF69.h>
extern RH_RF69 rf69;
uint32_t last_groundspeak = 0;

//snow
#include <Os/Log.hpp>

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

  U32 last_log_flush = 0;
  SDFile log_file;
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

    //now check to see if this should be in a telemetry log because we are in flight mode
    //TODO real way to determine sys mode with port to fsw component
    Types::SYS_MODE mode;
    Fw::ParamValid valid;
    //this->modeRequest_out().invoke(&mode, &valid);
    this->modeRequest_out(0, mode, valid);

    size_t status;

    if (valid && mode == Types::SYS_MODE::FLIGHT) {
      //check to see if log is already open and write to it if so
      if (!log_file) {
        char filename[15];
        //determine what the log file name should really be
        status = snprintf(filename, 15, "log_idx.log");
        log_file = SD.open(filename, FILE_READ);
        if (log_file) {
          char buf[10];
          status = log_file.read(buf, 10);
          log_file.close();
          //return value?
          SD.remove(filename);
          if (status > 0) {
            I32 next_log_id = atoi(buf);
            if (next_log_id > 0) {
              //save new log_count file before changing filename
              status = snprintf(buf, 10, "%d", next_log_id+1);
              log_file = SD.open(filename, FILE_WRITE);
              if (log_file) {
                log_file.write(buf, status);
                log_file.close();
                status = snprintf(filename, 15, "fauro%d.log", next_log_id);
              }
            }
          }
        }
        //compare the contents of filename to determine if we have a valid filename
        if (strncmp(filename, "log_idx.log", 15) == 0) {
          //case where there is a match which means we were unsuccessful
          //use log #1 and write 2 in the file
          log_file = SD.open(filename, FILE_WRITE);
          if (log_file) {
            log_file.write("2", 1);
            log_file.close();
            status = snprintf(filename, 15, "fauro%d.log", 1);
          } else {
            //TODO mega super fail EVR
          }
        }

        //open log file
        Fw::LogStringArg string_filename = Fw::LogStringArg(filename);
        log_file = SD.open(filename, FILE_WRITE);
        if (log_file) {
          this->log_ACTIVITY_HI_TELEMETRY_LOG_OPENED(string_filename);
        } else {
          this->log_WARNING_LO_TELEMETRY_LOG_OPEN_FAIL(string_filename);
        }
      }
      
      status = log_file.write(radio_msg, sendBuffer.getSize());
      //determine if the log file should be flushed
      if (now() - last_log_flush > 500) {
        log_file.flush();
        last_log_flush = now();
      }
    } 
    
    if (mode != Types::SYS_MODE::FLIGHT && log_file) {
      char filename[15];
      strncpy(filename,log_file.name(),15);
      Fw::LogStringArg string_filename = Fw::LogStringArg(filename);
      log_file.close();
      this->log_ACTIVITY_HI_TELEMETRY_LOG_CLOSED(string_filename);
    }

    deallocate_out(0, sendBuffer);
    return Drv::SendStatus::SEND_OK;
  }

}
