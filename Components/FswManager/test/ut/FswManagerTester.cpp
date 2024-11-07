// ======================================================================
// \title  FswManagerTester.cpp
// \author laboratory10
// \brief  cpp file for FswManager component test harness implementation class
// ======================================================================

#include "FswManagerTester.hpp"
#include "pgmspace_stub.h"

uint8_t mock_flash_memory[];

namespace Components {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  FswManagerTester ::
    FswManagerTester() :
      FswManagerGTestBase("FswManagerTester", FswManagerTester::MAX_HISTORY_SIZE),
      component("FswManager")
  {
    this->initComponents();
    this->connectPorts();
    this->setupTestMemory();
  }

  FswManagerTester ::
    ~FswManagerTester()
  {

  }

  void FswManagerTester :: setupTestMemory() {
    // Initialize mock_flash_memory with data for testing
    memset(mock_flash_memory, 0xAA, sizeof(mock_flash_memory));
    memset(mock_flash_memory, 0x1A, 5000);
    memset(mock_flash_memory+5000, 0x2B, 5000);

    // Set ending sequences
    for (U8 byte = 0; byte < 14; byte++) {
      mock_flash_memory[sizeof(mock_flash_memory)-136+byte] = 0xFF;
    }
    mock_flash_memory[sizeof(mock_flash_memory)-136+14] = 0x00;
    mock_flash_memory[sizeof(mock_flash_memory)-136+15] = 0x00;
  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void FswManagerTester ::
    toDo()
  {
    this->sendCmd_FSW_IMAGE_CRC(0, 2);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FswManagerComponentBase::OPCODE_FSW_IMAGE_CRC, 2, Fw::CmdResponse::OK);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_IMAGE_CRC_RESULT_SIZE(1);
    ASSERT_EVENTS_IMAGE_CRC_RESULT(0, sizeof(mock_flash_memory)-0x2000, 0x532FDCF);
  }

}
