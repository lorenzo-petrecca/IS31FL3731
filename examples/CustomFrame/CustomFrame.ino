#include "IS31FL3731.hpp"

/*
 *  CustomFrame Example
 *  
 *  This example shows how to:
 *      - Manually define frame data
 *      - Send raw pixel data to drivers
 */


IS31FL3731 screen;

#define ADDR_1 0x77
#define ADDR_2 0x74

DriverInfo *d1;
DriverInfo *d2;


uint8_t smile[LED_CONTROL_REGISTERS] = { 0xc0, 0x03, 0x20, 0x04, 0x10, 0x08, 0x48, 0x12, 0x08, 0x10, 0x48, 0x12, 0x90, 0x09, 0x20, 0x04, 0xc0, 0x03 };

uint8_t heart[LED_CONTROL_REGISTERS] = { 0x70, 0x1c, 0xf8, 0x3e, 0xf8, 0x3f, 0xf8, 0x3f, 0xf0, 0x1f, 0xe0, 0x0f, 0xc0, 0x07, 0x80, 0x03, 0x00, 0x01 };

void setup() {
    screen.init(ADDR_1, ADDR_2);

    d1 = screen.getDriverByAddress(ADDR_1);
    d2 = screen.getDriverByAddress(ADDR_2);
    
    screen.setIntensity_g(7, FRAME_1); // set the screen light intensity (all drivers)

    screen.allocateFrame(d1, FRAME_1);
    screen.allocateFrame(d2, FRAME_1);

    screen.clear();
    
    screen.setFramePixels(d1, FRAME_1, smile);
    screen.setFramePixels(d2, FRAME_1, heart);

    screen.display();
}

void loop() {
  
}
