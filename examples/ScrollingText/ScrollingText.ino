#include <IS31FL3731.hpp>

/*
 *  ScrollingText Example
 *  
 *  This example demonstrates:
 *      - Non-blocking scrolling text
 *      - Multi-driver text rendering
 * 
 */

IS31FL3731 screen;

#define ADDR_1 0x77
#define ADDR_2 0x74

DriverInfo *d1;
DriverInfo *d2;

void setup() {
    screen.init(ADDR_1, ADDR_2);

    d1 = screen.getDriverByAddress(ADDR_1);
    d2 = screen.getDriverByAddress(ADDR_2);

    screen.setIntensity_g(7, FRAME_1); // set the screen light intensity (all drivers)

    screen.allocateFrame(d1, FRAME_1);
    screen.allocateFrame(d2, FRAME_1);

    screen.clear();
}

void loop() {
    // Scroll text across both drivers
    screen.scrollText("Hello World", 500, d1, d2);
}