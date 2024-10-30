// ======================================================================
// \title  TelemetryManager.cpp
// \author laboratory10
// \brief  cpp file for TelemetryManager component implementation class
// ======================================================================

#include "Components/TelemetryManager/TelemetryManager.hpp"
#include "FpConfig.hpp"
#include <SD.h>
#include <TimeLib.h>
#include <Components/FswManager/FswManager.hpp>
#include <Os/Log.hpp>
#include <Arduino.h>
#include <RH_RF69.h>
#include <delay.h>
#include <wiring.h>

namespace Components {

  #define RESET_PIN         (12)
  #define PACKET_SIZE       (60)
  #define LOG_NAME_SIZE     (15)
  #define GROUNDSPEAK_DELAY (500)
  extern RH_RF69 radio;
  U32 last_groundspeak = 0;

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

  void TelemetryManager :: schedIn_handler(
    NATIVE_INT_TYPE portNum, 
    NATIVE_UINT_TYPE context) 
  {
    Fw::Buffer recv_buffer = this->allocate_out(0, SERIAL_BUFFER_SIZE);
    read_data(recv_buffer);
    if (millis() - last_groundspeak > GROUNDSPEAK_DELAY) {
      U8 message[PACKET_SIZE];
      U8 packet_size = PACKET_SIZE;
      memset(message, '\0', PACKET_SIZE);
      message[0] = '-';
      //There isn't much that can be done if this fails, don't check return
      (void)radio.send(message, PACKET_SIZE);
      memset(message, '\0', PACKET_SIZE);
      (void)radio.waitAvailableTimeout(PACKET_SIZE);
      if (radio.available()) {
        if (radio.recv(message, &packet_size)) {
          memcpy(recv_buffer.getData(), message, packet_size);
          recv_buffer.setSize(packet_size);
          this->tlmWrite_LAST_RSSI(radio.lastRssi());
        }
      }
      last_groundspeak = millis();
    }
    recv_out(0, recv_buffer, Drv::RecvStatus::RECV_OK);
  }

  U32 last_log_flush = 0;
  SDFile log_file;
  Drv::SendStatus TelemetryManager :: send_handler(
    NATIVE_INT_TYPE portNum, 
    Fw::Buffer& sendBuffer) 
  {
    write_data(sendBuffer);
    U8 radio_msg[PACKET_SIZE*3];
    memset(radio_msg, '\0', PACKET_SIZE*3);
    memcpy(radio_msg, sendBuffer.getData(), sendBuffer.getSize());
    //why are these delays necessary? 
    //ugh idk...best guess is the other radio is too slow printing to serial
    if (sendBuffer.getSize() <= PACKET_SIZE) {
      (void)radio.send(radio_msg, sendBuffer.getSize());
      delay(5);
    } else if (sendBuffer.getSize() <= PACKET_SIZE*2) {
      (void)radio.send(radio_msg, PACKET_SIZE);
      delay(5);
      (void)radio.send(radio_msg+PACKET_SIZE, PACKET_SIZE);
      delay(5);
    } else {
      (void)radio.send(radio_msg, PACKET_SIZE);
      delay(5);
      (void)radio.send(radio_msg+PACKET_SIZE, PACKET_SIZE);
      delay(5);
      (void)radio.send(radio_msg+(PACKET_SIZE*2), PACKET_SIZE);
      delay(5);
    }
    this->fatalPing_out(0);

    //now check to see if this should be in a telemetry log base on sys_mode
    Types::SYS_MODE mode;
    Fw::ParamValid valid;
    this->modeRequest_out(0, mode, valid);

    size_t status;

    if (valid && mode == Types::SYS_MODE::FLIGHT) {
      //check to see if log is already open and write to it if so
      //failing this conditional means no log file is open and we can continue
      if (!log_file) {
        char filename[LOG_NAME_SIZE];
        //determine what the log file name should really be
        status = snprintf(filename, LOG_NAME_SIZE, "log_idx.log");
        FW_ASSERT(status > 0);
        FW_ASSERT(status < LOG_NAME_SIZE);
        log_file = SD.open(filename, FILE_READ);
        //failing this conditional means the log index doesn't exist
        //we don't need an else as we will handle this below
        if (log_file) {
          char buf[LOG_NAME_SIZE];
          status = log_file.read(buf, LOG_NAME_SIZE);
          FW_ASSERT(status < LOG_NAME_SIZE);
          log_file.close();
          FW_ASSERT(SD.remove(filename));
          if (status > 0) {
            I32 next_log_id = atoi(buf);
            if (next_log_id > 0) {
              //save new log_count file before changing filename
              status = snprintf(buf, LOG_NAME_SIZE, "%d", next_log_id+1);
              FW_ASSERT(status > 0);
              FW_ASSERT(status < LOG_NAME_SIZE);
              log_file = SD.open(filename, FILE_WRITE);
              if (log_file) {
                FW_ASSERT(log_file.write(buf, status) == status);
                log_file.close();
                status = snprintf(
                  filename, 
                  LOG_NAME_SIZE, 
                  "fauro%d.log", 
                  next_log_id);
                FW_ASSERT(status > 0);
                FW_ASSERT(status < LOG_NAME_SIZE);
              } else {
                //we failed to open what should have been a unique log name
                this->log_WARNING_HI_UPDATE_LOG_INDEX_FAILED(filename); 
              }
            }
          }
        }
        //compare the contents of filename to see if we have a valid filename
        if (strncmp(filename, "log_idx.log", LOG_NAME_SIZE) == 0) {
          //case where there is a match which means we were unsuccessful
          //use log #1 and write 2 in the file
          log_file = SD.open(filename, FILE_WRITE);
          if (log_file) {
            log_file.write("2", 1);
            log_file.close();
            status = snprintf(filename, 15, "fauro%d.log", 1);
          } else {
            this->log_WARNING_HI_INIT_LOG_INDEX_FAILED();
          }
        }

        //open log file
        Fw::LogStringArg string_filename = Fw::LogStringArg(filename);
        log_file = SD.open(filename, FILE_WRITE);
        if (log_file) {
          this->log_ACTIVITY_HI_TELEMETRY_LOG_OPENED(string_filename);
        } else {
          this->log_WARNING_LO_TELEMETRY_LOG_OPEN_FAILED(string_filename);
        }
      }
      
      status = log_file.write(radio_msg, sendBuffer.getSize());
      //determine if the log file should be flushed
      if (now() - last_log_flush > GROUNDSPEAK_DELAY) {
        log_file.flush();
        last_log_flush = now();
      }
    } 
    
    if (mode != Types::SYS_MODE::FLIGHT && log_file) {
      char filename[LOG_NAME_SIZE];
      (void)strncpy(filename,log_file.name(),LOG_NAME_SIZE);
      Fw::LogStringArg string_filename = Fw::LogStringArg(filename);
      log_file.close();
      this->log_ACTIVITY_HI_TELEMETRY_LOG_CLOSED(string_filename);
    }

    deallocate_out(0, sendBuffer);
    return Drv::SendStatus::SEND_OK;
  }

}
