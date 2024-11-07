// pgmspace_stub.h

#ifndef PGM_SPACE_STUB_H
#define PGM_SPACE_STUB_H

#include <cstdint>

// Define a mock memory array to simulate flash memory for unit testing
extern uint8_t mock_flash_memory[0x40000];

// Mock pgm_read_byte to read from mock_flash_memory
inline uint8_t pgm_read_byte(uint32_t address) {
    // Ensure address is within bounds
    if (address < sizeof(mock_flash_memory)) {
        return mock_flash_memory[address];
    }
    // Return 0xFF or another value to simulate uninitialized or empty memory
    return 0xFF;
}

#define PROGMEM  // Empty macro for compatibility

#endif // PGM_SPACE_STUB_H
