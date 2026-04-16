#include <IS31FL3731.hpp>

/*
 *  BasicText Example
 *  This example shows how to:
 *      - Initialize two IS31FL3731 drivers
 *      - Allocate frame memory
 *      - Write simple text
 *      - Display it on the matrices
 */

IS31FL3731 screen;

#define ADDR_1 0x77
#define ADDR_2 0x74

DriverInfo *d1;
DriverInfo *d2;

void setup() {
    screen.init(ADDR_1, ADDR_2);

    // Retrieve driver instances
    d1 = screen.getDriverByAddress(ADDR_1);
    d2 = screen.getDriverByAddress(ADDR_2);

    screen.setIntensity_g(7, FRAME_1); // set the screen light intensity (all drivers)

    // Allocate frame memory (required for buffered mode)
    screen.allocateFrame(d1, FRAME_1);
    screen.allocateFrame(d2, FRAME_1);

    // Clear display
    screen.clear();

    // Write text (2 characters per driver)
    screen.write("12", d1, true);
    screen.write("45", d2);

    // Send data to hardware
    screen.display();
}

void loop() {

}