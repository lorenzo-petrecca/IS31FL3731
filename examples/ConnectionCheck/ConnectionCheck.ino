#include <IS31FL3731.hpp>

/*
 *  ConnectionCheck Example
 *  
 *  This example shows how to check whether the initialized
 *  IS31FL3731 drivers are correctly responding on the I2C bus.
 *  
 *  The result is printed on the Serial Monitor.
 *  
 *  check_connection() returns:
 *      0 -> success
 *      1 -> data too long for buffer
 *      2 -> address sent, NACK received
 *      3 -> data sent, NACK received
 *      4 -> other TWI error
 *      5 -> timeout
 */

IS31FL3731 screen;

#define ADDR_1 0x77
#define ADDR_2 0x74

DriverInfo *d1;
DriverInfo *d2;

void setup () {
    Serial.begin(115200);
    delay(1000);

    screen.init(ADDR_1, ADDR_2);

    d1 = screen.getDriverByAddress(ADDR_1);
    d2 = screen.getDriverByAddress(ADDR_2);

    int check1 = -1, check2 = -1;
    while (check1 != 0 && check2 != 0) {
        check1 = screen.check_connection(*d1);
        check2 = screen.check_connection(*d2);
        Serial.print("Check i2c communication at address ");
        Serial.print(ADDR_1);
        Serial.print("...");
        check1 == 0 ? Serial.println("Connection established.") : Serial.printf("Error in twi connection: err.code: %d.", check1);
        Serial.print("Check i2c communication at address ");
        Serial.print(ADDR_2);
        Serial.print("...");
        check2 == 0 ? Serial.println("Connection established.") : Serial.printf("Error in twi connection: err.code: %d.", check2);
        delay(2000);
    }
}

void loop () {
    
}