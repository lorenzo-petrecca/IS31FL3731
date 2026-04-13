/*
================================================================
Library name: IS31FL3731
Author: Lorenzo Petrecca
Version: 1.0.0
================================================================
*/
/*
*   This is a library to control one or more (up to 4) IS31FL3731 LED Drivers.
*   Each driver controls 144 LEDs, arranged in two 9x8 (9 rows and 8 columns) matrix in charlieplexing configuration.
*   Drivers can be assigned on of the following 4 addresses, depending how the AD pin is connected:
*       - 0x74 : AD pin connected to GND
*       - 0x77 : AD pin connected to VCC
*       - 0x75 : AD pin connected to SCL
*       - 0x76 : AD pin connected to SDA
*/

#pragma once

#include "Arduino.h"
#include "Wire.h"
#include <math.h>
#include "Font.hpp"
#include "font8x5.hpp"



#define FUNCTION_REGISTER 0x0B
#define COMMAND_REGISTER 0xFD
#define MAX_DRIVERS 4
#define LED_CONTROL_REGISTERS 18
#define MAX_FRAMES 8

enum Frame : uint8_t {
    FRAME_1 = 0x00,
    FRAME_2 = 0x01,
    FRAME_3 = 0x02,
    FRAME_4 = 0x03,
    FRAME_5 = 0x04,
    FRAME_6 = 0x05,
    FRAME_7 = 0x06,
    FRAME_8 = 0x07,
    NO_SELECTION = 0xFF
};

enum function : uint8_t {
    CONFIG_MODE = 0x00,  // Configure the operation mode
    PICTURE_FRAME = 0X01,   // Set the display frame in Picture Mode
    AUTO_PLAY_1 = 0X02, // Set the way of display in Auto Frame Play Mode
    AUTO_PLAY_2 = 0X03, // Set the delay time in Auto Frame Play Mode
    DISPLAY_OPTION = 0X05,  // Set the display option
    AUDIO_SYNC = 0X06,  // Set audio synchronization function
    FRAME_STATE = 0X07, // Store the frame display information
    BREATH_CTRL_1 = 0X08,   // Set fade in and fade out time for breath function
    BREATH_CTRL_2 = 0X09,   // Set the breath function
    SHUTDOWN = 0X0A,    // Set software shutdown mode
    AGC = 0X0B, // Set the AGC function and the audio gain
    _ADC = 0X0C  // Set the ADC sample rate of the input signal
};

// Configure the operation mode
enum Mode : uint8_t {
    PICTURE_MODE = 0x00,
    AUTO_FRAME_PLAY_MODE = 0x08,
    AUDIO_FRAME_PLAY_MODE = 0x10
};


// This class represents a single register frame for a device, consisting of a static 9x2 matrix.
class MatrixFrame {
    public:
        uint8_t data[LED_CONTROL_REGISTERS] = {B00000000};

        // This method is used to clear the matrix
        void clear() {
            for (unsigned int m = 0; m < LED_CONTROL_REGISTERS; ++m) {
                data[m] = B00000000;
            } 
        }
        
        // This method set a single row in matrix frame to a specific value
        bool setRow (uint8_t row, uint8_t value) {
            if (row < LED_CONTROL_REGISTERS) {
                data[row] = value;
                return true;
            }
            return false;
        }
};

// Information structure for every device
struct DriverInfo {
    uint8_t address; // i2c device address
    Mode operating_mode; // device operating mode
    Frame picture_frame; // device picture frame
    MatrixFrame *frames[MAX_FRAMES]; // Array of pointer to frames 

    DriverInfo (uint8_t addr, Mode mode, Frame frame)
        : address(addr), operating_mode(mode), picture_frame(frame) {
        for (auto &f : frames) {
            f = nullptr;
        }
    }
};


class IS31FL3731 {
    private:
        DriverInfo *drivers[MAX_DRIVERS] = {nullptr};
        uint8_t configMode (Mode mode, Frame frame = FRAME_1);
        Font font;
        int writeDriver (DriverInfo *info);
        uint8_t selectPage (uint8_t addr, uint8_t page);
        uint8_t writeRegister (uint8_t addr, uint8_t reg, uint8_t data);
        uint8_t i2c_selectAndWrite (uint8_t addr, uint8_t page, uint8_t reg, uint8_t data);
        void clearFrameInDriver (DriverInfo *driver, Frame frame);
        uint8_t reverseByte (uint8_t byte);

    public:
        IS31FL3731() : font(&font8x5) {};
        void set_address (uint8_t *addr1, uint8_t *addr2 = nullptr, uint8_t *addr3 = nullptr, uint8_t *addr4 = nullptr);
        uint8_t check_connection (uint8_t addr);
        uint8_t check_connection (DriverInfo driver);
        uint8_t init (uint8_t addr1 = 0xff, uint8_t addr2 = 0xff, uint8_t addr3 = 0xff, uint8_t addr4 = 0xff);
        uint8_t init (int number_of_drivers);
        int setOperatingMode (DriverInfo *driver, Mode mode, Frame frame = FRAME_1);
        void setOperatingMode_g (Mode mode, Frame frame = FRAME_1);
        int setStartingFrame (DriverInfo *driver, Frame frame);
        void setStartingFrame_g (Frame frame);
        int setIntensity (DriverInfo *driver, uint8_t pwm_register, unsigned int intensity, Frame frame = FRAME_1);
        int setIntensity (DriverInfo *driver, unsigned int intensity, Frame frame = FRAME_1);
        void setIntensity_g (unsigned int intensity, Frame frame = FRAME_1);
        bool writeFrame (uint8_t ledRegisters[MAX_DRIVERS][LED_CONTROL_REGISTERS], uint8_t frame = FRAME_1);
        bool writeFrame (DriverInfo *driver, uint8_t ledRegisters[LED_CONTROL_REGISTERS], uint8_t frame = FRAME_1);
        bool allocateFrame (DriverInfo *driver, Frame frame);
        bool freeFrame (DriverInfo *driver, Frame frame);
        bool freeAllFrames (DriverInfo *driver);
        bool setFramePixels (DriverInfo *driver, Frame frame, uint8_t *data);
        void write (const char *str, DriverInfo *driver = nullptr, bool highlight = false);
        void setFont(FontType *font);
        int display (DriverInfo *driver = nullptr);

        DriverInfo* getDriverByAddress (uint8_t addr);
        int shutdown_mode (DriverInfo *driver, bool enable);
        void shutdown_mode_g (bool enable);
        void clear (Frame frame = NO_SELECTION, DriverInfo *driver = nullptr);
        bool scrollText (const char *str, unsigned long interval, DriverInfo *driver_1 = nullptr, DriverInfo *driver_2 = nullptr, DriverInfo *driver_3 = nullptr, DriverInfo *driver_4 = nullptr);
        bool clearFramePixels(DriverInfo *driver, Frame frame);
        // bool scrollPixel(const char *str, unsigned long interval, DriverInfo *driver_1 = nullptr, DriverInfo *driver_2 = nullptr, DriverInfo *driver_3 = nullptr, DriverInfo *driver_4 = nullptr);

        ~IS31FL3731();
};


