#include "IS31FL3731.hpp"

/*
 *  DirectWriteFrame Example
 *  
 *  This example demonstrates the direct mode of the library.
 *  
 *  In this mode:
 *      - frame data is written directly to the driver registers
 *      - no RAM frame allocation is required
 *      - display() is not required
 *  
 *  This is useful for:
 *      - quick tests
 *      - raw patterns
 *      - low-memory usage
 */

IS31FL3731 screen;

#define ADDR_1 0x77
#define ADDR_2 0x74


uint8_t test_frame[][LED_CONTROL_REGISTERS] = {
    { 0xc0, 0x03, 0x20, 0x04, 0x10, 0x08, 0x48, 0x12, 0x08, 0x10, 0x48, 0x12, 0x90, 0x09, 0x20, 0x04, 0xc0, 0x03 }, // smile
    { 0xc0, 0x03, 0x20, 0x04, 0x10, 0x08, 0x48, 0x12, 0x08, 0x10, 0x88, 0x11, 0x50, 0x0a, 0x20, 0x04, 0xc0, 0x03 }, // sad
};

void setup() {
  screen.init(ADDR_1, ADDR_2);
  
  // select operating mode (all drivers)
  screen.setOperatingMode_g(PICTURE_MODE);  
  // select starting frame (all drivers)
  screen.setStartingFrame_g(FRAME_1);
  // set the screen light intensity (all drivers)
  screen.setIntensity_g(7, FRAME_1); // setta l'intensità luminosa dello schermo

  delay(50);

  screen.writeFrame(test_frame, FRAME_1);
}

void loop() {
  
}
