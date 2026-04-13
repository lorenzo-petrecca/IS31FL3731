#include "IS31FL3731.hpp"

// =========================================================================
/*
    twi library error table:
        * 0 : success 
        * 1 : length to long for buffer
        * 2 : address send, NACK received
        * 3 : data send, NACK received
        * 4 : other twi error (lost bus arbitration, bus error, ..)
        * 5 : timeout
*/
// =========================================================================


uint8_t IS31FL3731::reverseByte(uint8_t byte) {
    byte = (byte & 0xF0) >> 4 | (byte & 0x0F) << 4;
    byte = (byte & 0xCC) >> 2 | (byte & 0x33) << 2;
    byte = (byte & 0xAA) >> 1 | (byte & 0x55) << 1;
    return byte;
}



/* 
    Set the address of i2c device
    Up to 4 addresses can be set
*/
void IS31FL3731::set_address (uint8_t *addr1, uint8_t *addr2, uint8_t *addr3, uint8_t *addr4) {
    uint8_t *addresses[MAX_DRIVERS] = {addr1, addr2, addr3, addr4};
    for (uint8_t i = 0; i < MAX_DRIVERS; ++i) {
        if (this->drivers[i] != nullptr) {
            this->drivers[i]->address = addresses[i] != nullptr ? *addresses[i] : this->drivers[i]->address;
        }
    }
}

uint8_t IS31FL3731::configMode (Mode mode, Frame frame) {
    if (mode != PICTURE_MODE) {
        return mode || frame;
    }
    return PICTURE_MODE;
}


/*
    Check if the i2c connection is established
    Return: 0 if true otherwise return the Wire library error code
    Input: address at i2c connection you want to check
    Note: this function must be called only after the init function has been called
*/
uint8_t IS31FL3731::check_connection (uint8_t addr) {
    Wire.beginTransmission(addr);
    uint8_t status = Wire.endTransmission();
    return status;
}

/*
    Check if the driver is connected to i2c
    Return: 0 if true otherwise return the Wire library error code
    Input: DriverInfo structure
    Note: this function must be called only after the init function has been called
*/
uint8_t IS31FL3731::check_connection (DriverInfo driver) {
    return check_connection(driver.address);
}

/*
    Function: setOperatingMode
    Desc: Set the operation mode for current driver (Default mode is PICTURE_MODE)
    Note: If you want to set PICTURE_MODE like opeating mode, you should not specify the frame because
        it's not available at this stage and it will be automatically set to 0x00.
        Instead if you want to set AUTO_FRAME_PLAY_MODE you have to specify
        frame otherwise it will be automatically set to FRAME_1.
    Input: driver - operating mode - frame
    Valid operating mode input:
        - PICTURE_MODE
        - AUTO_FRAME_PLAY_MODE
        - AUDIO_FRAME_PLAY_MODE
    Output: (int) status:
            * -1 : driver is not initialized
            * twi error table
*/
int IS31FL3731::setOperatingMode (DriverInfo *driver, Mode mode, Frame frame) {
    if (driver != nullptr) { 
        uint8_t status = i2c_selectAndWrite(driver->address, FUNCTION_REGISTER, CONFIG_MODE, (Mode)configMode(mode, frame));
        if (status == 0) driver->operating_mode = (Mode)configMode(mode, frame);
        return status;
    }
    return -1;
}

/*
    Function: setOperatingMode_g
    Desc: Set the operation mode for all drivers (Default mode is PICTURE_MODE)
    Note: If you want to set PICTURE_MODE like opeating mode, you should not specify the frame because
        it's not available at this stage and it will be automatically set to 0x00.
        Instead if you want to set AUTO_FRAME_PLAY_MODE or AUDIO_FRAME_PLAY_MODE you have to specify
        frame otherwise it will be automatically set to FRAME_1.
    Input: operating mode - frame
    Valid operating mode input:
        - PICTURE_MODE
        - AUTO_FRAME_PLAY_MODE
        - AUDIO_FRAME_PLAY_MODE
*/
void IS31FL3731::setOperatingMode_g (Mode mode, Frame frame) {
    for (int i = 0; i < MAX_DRIVERS; ++i) {
        setOperatingMode(this->drivers[i], mode);
    }
}

/*
    Function: setStartingFrame
    Desc: Set the current driver picture frame register from where the driver will start reading data
    Input: driver - frame
    Note: If you setted the operating mode in AUTO_FRAME_PLAY_MODE or in AUDIO_FRAME_PLAY_MODE
        you should not call this function.
    Output: (int) status:
            * -1 : driver is not initialized
            * twi error table
*/
int IS31FL3731::setStartingFrame (DriverInfo *driver, Frame frame) {
    if (driver != nullptr) {
        uint8_t status = i2c_selectAndWrite(driver->address, FUNCTION_REGISTER, PICTURE_FRAME, frame);
        if (status == 0) driver->picture_frame = frame;
        return status;
    }
    return -1;
}

/*
    Function: setStartingFrame_g
    Desc: Set all drivers picture frame register from where the driver will start reading data
    Input: frame
    Note: If you setted the operating mode in AUTO_FRAME_PLAY_MODE or in AUDIO_FRAME_PLAY_MODE
        you should not call this function.
*/
void IS31FL3731::setStartingFrame_g (Frame frame) {
    for (int i = 0; i < MAX_DRIVERS; ++i) {
        setStartingFrame(drivers[i], frame);
    }
}

/*
    Function: init
    Desc: This function initialize drivers. Sets drivers operating mode on PICTURE_MODE by default
    If you want to change drivers operating mode you must call setOperatingMode_g function
    Otherwise if you want to change single driver operating mode you must call setOperatingMode function.
    Input: address of each driver
    Output: (uint8_t) number of drivers currently initialized

*/
uint8_t IS31FL3731::init (uint8_t addr1, uint8_t addr2, uint8_t addr3, uint8_t addr4) {
    uint8_t addresses[4] = {addr1, addr2, addr3, addr4};
    //this->font = Font(&font8x5); // create instance of default font
    uint8_t count;
    Wire.begin();
    Wire.setClock(400000);
    for (uint8_t i = 0; i < MAX_DRIVERS; ++i) {
        if (addresses[i] != 0xff) {
            this->drivers[i] = new DriverInfo(addresses[i], PICTURE_MODE, FRAME_1);
            if (check_connection(*this->drivers[i]) == 0) {
                ++count;
            }
            // shutdown mode to correctly initilize drivers
            shutdown_mode(drivers[i], true);    // put drivers in shutdown mode
            delay(10);
            shutdown_mode(drivers[i], false);   // exit from shutdown mode

            // setting operating mode to the current default driver (DriverInfo) operating mode 
            setOperatingMode(this->drivers[i], this->drivers[i]->operating_mode);
            clear(); // clear all frames in all drivers
        }
    }
    return count;
}
/*
    Function: init
    Desc: This function initialize drivers. Sets drivers operating mode on PICTURE_MODE by default
    If you want to change drivers operating mode you must call setOperatingMode_g function
    Otherwise if you want to change single driver operating mode you must call setOperatingMode function.
    Input: (uint8_t) number_of_drivers
    Output: (uint8_t) number of drivers currently initialized
    Note: This method automatically assign default i2c addresses for number of drivers specified
*/
uint8_t IS31FL3731::init (int number_of_drivers) {
    uint8_t addresses[MAX_DRIVERS] = {0xFF};
    if (number_of_drivers > 0 && number_of_drivers <= 4) {
        switch (number_of_drivers) {
            case 1:
                addresses[0] = 0x74;
                break;
            case 2:
                addresses[0] = 0x74;
                addresses[1] = 0x77;
                break;    
            case 3:
                addresses[0] = 0x74;
                addresses[1] = 0x77;
                addresses[2] = 0x75;
                break;    
            case 4:
                addresses[0] = 0x74;
                addresses[1] = 0x77;
                addresses[2] = 0x75;
                addresses[3] = 0x76;
                break;    
            default:
                return 0; // UNKNOWN ERROR
                break;
        }

        //this->font = Font(&font8x5); // create instance of default font
        uint8_t count;
        Wire.begin();
        Wire.setClock(400000);
        for (uint8_t i = 0; i < MAX_DRIVERS; ++i) {
            if (addresses[i] != 0xff) {
                this->drivers[i] = new DriverInfo(addresses[i], PICTURE_MODE, FRAME_1);
                if (check_connection(*this->drivers[i]) == 0) {
                    ++count;
                }
                // shutdown mode to correctly initilize drivers
                shutdown_mode(drivers[i], true);    // put drivers in shutdown mode
                delay(10);
                shutdown_mode(drivers[i], false);   // exit from shutdown mode

                // setting operating mode to the current default driver (DriverInfo) operating mode 
                setOperatingMode(this->drivers[i], this->drivers[i]->operating_mode);
                clear(); // clear all frames in all drivers
            }
        }
        return count;
    }
    return 0;
}

/*
    Function: setIntensity
    Desc: Sets the intensity of single LED
    Input: driver - pwm Register (LED) - intensity - frame
    Note: 
        - LED Value (register) must be in range 0x24 ~ 0xB3
        - Intensity value must be in range 0 ~ 100
        - frame is default setted to FRAME_1
    Output: (int) status:
            * -1 : driver is not initialized
            * twi error table
*/
int IS31FL3731::setIntensity (DriverInfo *driver, uint8_t pwm_register, unsigned int intensity, Frame frame) {
    if (driver != nullptr) {
        uint8_t pwm = round((intensity * 255) / 100);

        uint8_t status = i2c_selectAndWrite(driver->address, frame, pwm_register, pwm);
        return status;
    }

    return -1;
}

/*
    Function: setIntensity
    Desc: Sets the intensity of each LED
    Input: driver - intensity - frame
    Note: 
        - This function set all LEDs intensity at the same value
        - Intensity value must be in range 0 ~ 100
        - frame is default setted to FRAME_1
    Output: (int) status:
            * -1 : driver is not initialized
            * twi error table
*/
int IS31FL3731::setIntensity (DriverInfo *driver, unsigned int intensity, Frame frame) {
    if (driver != nullptr) {
        uint8_t pwm = round((intensity * 255) / 100);
        uint8_t pwm_register = 0x24;
        uint8_t status = selectPage(driver->address, frame);
        if (status != 0) return status;
        for (unsigned int i = 0; i < LED_CONTROL_REGISTERS * 8; ++i) {
          status = writeRegister(driver->address, pwm_register++, pwm);
          if (status != 0) return status;
        }
        return 0;
    }
    return -1;
}

/*
    Function: setIntensity_g
    Desc: Sets the intensity of each LED for all drivers
    Input: intensity - frame
    Note: 
        - This function set all LEDs intensity at the same value for all drivers
        - Intensity value must be in range 0 ~ 100
        - frame is default setted to FRAME_1
*/
void IS31FL3731::setIntensity_g (unsigned int intensity, Frame frame) {
    for (unsigned int i = 0; i < MAX_DRIVERS; ++i) {
        setIntensity(this->drivers[i], intensity, frame);
    }
}

/*
    This function available to write in initialized driver registers
    Input: On/off led matrices - frame register to start writing from (default) FRAME_1
    Output: On success true, false otherwise
    Note: This function should be called only for testing purposes, as it does not save data to the 
        driver's frame array, but only writes to the driver's frame register to directly display the sent bytes.
        If you want to have more control over the driver with this library, you should call other library methods
        so as to allocate memory for your byte arrays. However if you want to preserve controller memory,  
        its useful to call this function.
*/
bool IS31FL3731::writeFrame (uint8_t ledRegisters[MAX_DRIVERS][LED_CONTROL_REGISTERS], uint8_t frame) {
    int responses[MAX_DRIVERS] = {-1};
    for (int i = 0; i < MAX_DRIVERS; ++i) {
        if (drivers[i] != nullptr) {
            uint8_t status = selectPage(drivers[i]->address, frame);
            if (status != 0) {
                responses[i] = 0;
                continue;
            }
            for (int r = 0; r < LED_CONTROL_REGISTERS; ++r) {
                status = writeRegister(drivers[i]->address, r, ledRegisters[i][r]);
                if (status != 0) {
                    responses[i] = 0;
                    break;
                }
            }
            responses[i] =  1;
        } else {
            responses[i] = 1;
        }
    }
    return responses[0] && responses[1] && responses[2] && responses[3];
}

bool IS31FL3731::writeFrame (DriverInfo *driver, uint8_t ledRegisters[LED_CONTROL_REGISTERS], uint8_t frame) {
    if (driver != nullptr) {
        if (selectPage(driver->address, frame) != 0) return 0;
        for (int r = 0; r < LED_CONTROL_REGISTERS; ++r) {
            if (writeRegister(driver->address, r, reverseByte(ledRegisters[r])) != 0) return 0;
        }
        return 1;
    }
    return 0;
}



/*
    Function: allocateFrame
    Desc: This function will allocate matrix data in memory for the given frame and the given driver
    Input: driver - frame
    Output: (bool) true if successfully allocated memory for the frame in current driver, false otherwise
    Note: This function only reserves memory for the frame in current driver, not sends it to the driver
*/
bool IS31FL3731::allocateFrame (DriverInfo *driver, Frame frame) {
    if (driver != nullptr && frame <= FRAME_8) {
        driver->frames[frame] = new MatrixFrame();
        return true;
    }
    return false;
}

/*
    Function: freeFrame
    Desc: Free the specified frame in the specified driver
    Input: driver - frame
    Output: (bool) true if memory was successfully freed, false otherwise
*/
bool IS31FL3731::freeFrame (DriverInfo *driver, Frame frame) {
    if (driver != nullptr && frame <= FRAME_8) {
        delete driver->frames[frame];
        driver->frames[frame] = nullptr;
        return true;
    }
    return false;
}

/*
    Function: freeAllFrames
    Desc: Free all frames in array, in the specified driver
    Input: driver
    Output: (bool) true if memory was successfully freed, false otherwise
*/
bool IS31FL3731::freeAllFrames (DriverInfo *driver) {
    if (driver != nullptr) {
        for (uint8_t f = 0; f < MAX_FRAMES; ++f) {
            freeFrame(driver, (Frame)f);
        }
        return true;
    }
    return false;
}

/*
    Function: setFramePixels
    Desc: set the matrix data to the specified frame in the specified driver
    Input: driver - frame - data
    Output: (bool) true if successfully, false otherwise
    Note: This function only writes in the frame array istance of the driver, it does not send data to the driver.
        If you want to send data to the driver you must call display() function, after setting up the frame array 
        correctly with this function.
*/
bool IS31FL3731::setFramePixels (DriverInfo *driver, Frame frame, uint8_t *data) {
    if (driver != nullptr && frame <= FRAME_8) {
        for (uint8_t b = 0; b < LED_CONTROL_REGISTERS; ++b) {
            driver->frames[frame]->setRow(b, data[b]);
        }
        return true;
    }
    return false;
}

/*
    Function: setFont
    Desc: Sets the font
    Input: font (FontType)
    Note: To set a font other than the default one, you need to add the font to the library, if it doesn't 
        already exist, include it in the main code and pass it by reference to this function.
        For more information read the README.md file.
*/
void IS31FL3731::setFont(FontType *font) {
    this->font.setCurrentFont(font);
}

/*
    Function setFrameTopPadding
    Desc: Sets the top padding of frames for the specified driver. 
        Affects the position of the character in the matrix.
    Input: driver - top padding
    Note: If you assign 0 to the top padding, the one line space will be applied only to the bottom padding. 
        If you assign 1, the opposite happens.
        If you do not call this function before calling the write function, then the last value assigned to the 
        top padding for the specified driver instance will be assumed. The default value of top padding is 0. 
        You can never call this function if you do not need to change the value.
*/
/*void IS31FL3731::setFrameTopPadding(DriverInfo *driver, bool topPadding) {
    if (driver != nullptr) {
        driver->topPadding = topPadding;
    }
}*/

/*
    Function: write
    Desc: Write to the frame buffer
    Input: const char *str (characters to print in drivers matrices)  | DriverInfo *driver (default: nullptr) | bool highlight (default: false)
    Note: This function only writes to the frame array at the current picture_frame position, but does not
        send data to the driver. If you want to send data you must call display() function after calling this function.
*/
void IS31FL3731::write (const char *str, DriverInfo *driver, bool highlight) {
    if (!str) return;
    size_t length = strlen(str);
    if (length == 0) return;

    DriverInfo *currentDriver;

    unsigned int cursor = 0;
    char buffer[2] = "";
    
    for (unsigned int d = 0; d < MAX_DRIVERS; ++d) {
        // assign current driver
        currentDriver = driver;

        // fill buffer
        buffer[0] = (cursor < length) ? str[cursor++] : '\0';
        buffer[1] = (cursor < length) ? str[cursor++] : '\0';
        
        // Controllo validità del driver
        // Driver validity check
        if (!this->drivers[d] || !this->drivers[d]->frames[this->drivers[d]->picture_frame]) {
            continue; // Jump this driver if it is not valid
        }

        // assegna a currentDriver il driver corrente se non è stato passato nessun driver specifico
        if(currentDriver == nullptr && this->drivers[d]) {
            currentDriver = this->drivers[d];
        }

        // Scrittura dei caratteri nel frame
        // Writing characters in frame
        for (uint8_t c = 0; c < 2; c++) {
            for (uint8_t f = 0 + c, b = 0; f < LED_CONTROL_REGISTERS && b < 8; f += 2, b++) {
                const uint8_t *characterData = this->font.getCharacter(buffer[c]);
                if (characterData) {
                    uint8_t data[8] = {0};

                    // reverse bytes
                    for (uint8_t byte = 0; byte < 8; byte++) {
                        data[byte] = highlight ? ~reverseByte(characterData[byte]) : reverseByte(characterData[byte]);
                    }
                    currentDriver->frames[currentDriver->picture_frame]->setRow(f, data[b]);
                }
            }
        }
        if (!highlight) {
            currentDriver->frames[currentDriver->picture_frame]->setRow(16, 0x00);      
            currentDriver->frames[currentDriver->picture_frame]->setRow(17, 0x00);  
        } else {
            currentDriver->frames[currentDriver->picture_frame]->setRow(16, 0xFF);      
            currentDriver->frames[currentDriver->picture_frame]->setRow(17, 0xFF);  
        }      
          
    }
}

/*
    Function: writeDriver
    Desc: This is a private function. It takes care of writing the driver's data frame
    Input: driver
    Note: This function only sends data to one driver. It's only called by display() public function.
    Output: (int) status:
            * -1 : driver is not initialized
            * twi error table
*/
int IS31FL3731::writeDriver (DriverInfo *driver) {
    if (driver != nullptr) {
        /*
        Wire.beginTransmission(driver->address);
        Wire.write(COMMAND_REGISTER);
        Wire.write(driver->picture_frame);
        */
        uint8_t status = selectPage(driver->address, driver->picture_frame);
        if (status != 0) return status;
        uint8_t curr_led_register = 0x00;
        for (uint8_t i = 0; i < LED_CONTROL_REGISTERS; ++i) {
            /*
            Wire.write(curr_led_register++);
            Wire.write(driver->frames[driver->picture_frame]->data[i]);
            */
            status = writeRegister(driver->address, curr_led_register++, driver->frames[driver->picture_frame]->data[i]);
            if (status != 0) return status;
        }
        return 0;
    }
    return -1;
}

/*
    Function: display
    Desc: Display the current frame data (write to driver frame register).
    Input: driver (Default: nullptr)
    Output: (int) number of devices successfully written
    Note: If the device is successfully written the number of bytes written will always be the number of
        LED_CONTROL_REGISTERS (even if the font character is not made up of 9 bytes).
*/
int IS31FL3731::display (DriverInfo *driver) {
    int num = 0;
    if (driver == nullptr) {
        int statusArr[MAX_DRIVERS] = {-1};
        for (int i = 0; i < MAX_DRIVERS; ++i) {
            num = writeDriver(this->drivers[i]) == 0 ? num+=1 : num;
        }
        
    } else {
        num = writeDriver(driver) == 0 ? 1 : 0;
    }
    return num;
}


/*
    Function: getDriverByAddress
    Desc: Return corresponding driver by passing addres
    Input: uint8_t addr
*/
DriverInfo* IS31FL3731::getDriverByAddress (uint8_t addr) {
    for (int i = 0; i < MAX_DRIVERS; ++i) {
        if (drivers[i] != nullptr && drivers[i]->address == addr) {
            return drivers[i];
        }
    }
    return nullptr;
}

/*
    Function: shutdown_mode
    Desc: Put selected driver in shutdown mode
    Input: DriverInfo driver | bool enable
*/
int IS31FL3731::shutdown_mode (DriverInfo *driver, bool enable) {
    if (driver != nullptr) {
        return i2c_selectAndWrite(driver->address, FUNCTION_REGISTER, SHUTDOWN, !enable);
    }
    return -1;
}

/*
    Function: shutdown_mode_g
    Desc: Put all drivers in shutdown mode
    Input: bool enable
*/
void IS31FL3731::shutdown_mode_g (bool enable) {
    for (unsigned int i = 0; i < MAX_DRIVERS; ++i) {
        shutdown_mode(drivers[i], enable);
    }
}


uint8_t IS31FL3731::selectPage (uint8_t addr, uint8_t page) {
    Wire.beginTransmission(addr);
    Wire.write(COMMAND_REGISTER);
    Wire.write(page);
    return Wire.endTransmission();
}


uint8_t IS31FL3731::writeRegister (uint8_t addr, uint8_t reg, uint8_t data) {
    Wire.beginTransmission(addr);
    Wire.write(reg);
    Wire.write(data);
    return Wire.endTransmission();
}

uint8_t IS31FL3731::i2c_selectAndWrite (uint8_t addr, uint8_t page, uint8_t reg, uint8_t data) {
    int s = selectPage(addr, page);
    if (s != 0) return s;
    int w = writeRegister(addr, reg, data);
    if (w != 0) return w;
    return 0;
}

/*
    Function: clear
    Desc: Clear selected frame setting all leds off
    Input: Frame frame (default: NO_SELECTION) | DriverInfo driver (default: nullptr)
    Note: if you do not specify frame will be clean all frames. And if you do not specify driver will be clean all drivers
*/
void IS31FL3731::clear (Frame frame, DriverInfo *driver) {
    if (driver == nullptr) {
        for (uint8_t i = 0; i < MAX_DRIVERS; ++i) {
            if (frame != NO_SELECTION) {
                clearFrameInDriver(drivers[i], frame);
                clearFramePixels(drivers[i], frame);
            } else {
                for (uint8_t f = FRAME_1; f <= FRAME_8; ++f) {
                    clearFrameInDriver(drivers[i], (Frame)f);
                    if (drivers[i]->frames[f] != nullptr) {
                        clearFramePixels(drivers[i], (Frame)f);
                    }
                }
            }
        }
    } else {
        if (frame != NO_SELECTION) {
            clearFrameInDriver(driver, frame);
            //clearFramePixels(driver, frame);
        } else {
            for (uint8_t f = FRAME_1; f <= FRAME_8; ++f) {
                clearFrameInDriver(driver, (Frame)f);
                if (driver->frames[f] != nullptr) {
                        clearFramePixels(driver, (Frame)f);
                }
            }
        }
    }
     
}
void IS31FL3731::clearFrameInDriver (DriverInfo *driver, Frame frame) {
    selectPage(driver->address, frame);
    for (uint8_t r = 0; r < LED_CONTROL_REGISTERS; ++r) {
        writeRegister(driver->address, r, 0x00);
    }
}

/*
    Function: scrollText
    Desc: Function to scroll text in display, by the number of drivers correctly initialized passed.
    Input: DriverInfo *driver_1, DriverInfo *driver_2, DriverInfo *driver_3, DriverInfo *driver_4
    Output: (bool) returns true if it has finished scrolling the text, otherwise false
    Note: Passare al metodo i driver su cui si vuole far scorrere il testo. è necessario aver inizializzato correttamente i driver che si vogliono utilizzare per eseguire lo scroll
    del testo tra le matrici dei vari driver. Il testo scorrerà da destra a sinistra ovvero dall'ultimo driver passato al driver 1.
    Il testo scorre di un offset fissato di un carattere. Qualora venga passato solo un driver il testo scorrerà sempre di
    un carattere alla volta sul singolo driver (ogni driver supporta due caratteri). 
*/
bool IS31FL3731::scrollText (const char *str, unsigned long interval, DriverInfo *driver_1, DriverInfo *driver_2, DriverInfo *driver_3, DriverInfo *driver_4) {
    if (!str) return true;
    size_t length = strlen(str);
    if (length == 0) return true;

    DriverInfo *ds[MAX_DRIVERS] = {driver_1, driver_2, driver_3, driver_4};
    uint8_t num_of_drivers = 0;

    // controllo numero di driver passati e inizializzati
    for (unsigned int d = 0; d < MAX_DRIVERS; ++d) {
        if (this->drivers[d] != nullptr && ds[d] != nullptr) num_of_drivers++;
    }
    if (num_of_drivers > MAX_DRIVERS || num_of_drivers == 0) return true;

    // controllo se la stringa può essere scritta nel display senza bisogno di scroll
    if (length <= num_of_drivers * 2) {
        write(str);
        return true;
    }

    static unsigned long previous = 0;
    static unsigned int offset = 0;
    if (interval < millis() - previous) {
        previous = millis();
        char buff[(2 * num_of_drivers) + 1];
        buff[2 * num_of_drivers] = '\0';
        for (unsigned int d = 0; d < num_of_drivers; ++d) {
            if (ds[d]) { 
                unsigned int c = offset + d * 2;  
                buff[d * 2] = c < length ? str[c] : ' ';
                buff[d * 2 + 1] = c < length ? str[c+1] : ' ';

                write(buff, ds[d]);
            }
        }
        
        display();
        // Increment offset
        if (++offset + num_of_drivers * 2 > length) {
            offset = 0; // Reset offset
            return true; // Scroll completed
        }
        
    }
    
    return false;   // scroll in progress
}


bool IS31FL3731::clearFramePixels (DriverInfo *driver, Frame frame) {
    if (driver != nullptr && frame <= FRAME_8) {
        /*for (uint8_t b = 0; b < LED_CONTROL_REGISTERS; ++b) {
            driver->frames[frame]->setRow(b, 0x00);
        }*/
        driver->frames[frame]->clear();
        return true;
    }
    return false;
}


/*

bool IS31FL3731::scrollPixel(const char *str, unsigned long interval, DriverInfo *driver_1, DriverInfo *driver_2, DriverInfo *driver_3, DriverInfo *driver_4) {
    // 1) se la stringa è tropo corta ritorna vero
    // 2) se il tempo non è trascorso ritorna falso
    // 3) esegui il controllo dei driver passati e se sono stati inizializzati 
    // 4) controllare se la stringa può essere stampata senza bisogno di scroll
    // 5) convertire la stringa in un array di pixel organizzati per colonne, in cui ogni elemento rappresenta una colonna
    // della stringa da convertire, usando un buffer salvato in memoria (memset).
    // 6) Per far ciò usare una variabile "buffer_initialized" per tenere traccia del buffer e sapere quando è pronto per iniziare lo scroll (settata inizialmente su false)
    // 7) Copiare i dati dei caratteri e salvarli nel buffer
    // 8) Inizia lo scroll e stampa i caratteri
    // 9) Incrementa lo scroll
    // 10) Se l'offset è maggiore o uguale della lunghezza del buffer riporta l'offset a 0 e "buffer_initialized" su false per poter iniziare un nuovo scroll
    // 11) display()

    if (!str) return true;
    size_t length = strlen(str);
    if (length == 0) return true;

    static unsigned long previous = 0;
    if (interval > millis() - previous) return false;
    previous = millis();

    DriverInfo *ds[MAX_DRIVERS] = {driver_1, driver_2, driver_3, driver_4};
    uint8_t num_of_drivers = 0;  

    // controllo numero di driver passati e inizializzati
    for (unsigned int d = 0; d < MAX_DRIVERS; ++d) {
        if (this->drivers[d] != nullptr && ds[d] != nullptr) num_of_drivers++;
    }
    if (num_of_drivers == 0) return true;

    DriverInfo *dsPass[num_of_drivers] = {nullptr}; // inizializza l'array di driver in cui saranno salvati quelli passati

    // controllo se la stringa può essere scritta nel display senza bisogno di scroll
    if (length <= num_of_drivers * 2) {
        write(str);
        return true;
    }

    // Dimensioni display
    const uint8_t display_height = LED_CONTROL_REGISTERS / 2; // altezza delle colonne
    const uint8_t display_width = num_of_drivers * 16;

    // Covertire stringa in un buffer di pixel (1 byte = 1 colonna) (max 1024 colonne = 128 caratteri)
    const size_t pixel_buffer_length = length * 8; // lunghezza della stringa in pixel
    static uint8_t pixel_buffer[1024][2] = {0, 0}; // istanza array della stringa in pixel
    static bool buffer_initialized = false;

    // scrittura nel buffer
    if (!buffer_initialized) {
        memset(pixel_buffer, 0, sizeof(pixel_buffer)); // inizializza il blocco di memoria per la stringa in pixel
        for (size_t i = 0; i < length; ++i) { //
            const uint8_t *char_data = font.getCharacter(str[i]); // ottieni bitmap del carattere
            if (!char_data) continue; // se carattere non disponibile ignora
            // copia la bitmap del carattere nel pixel_buffer in colonne
            for (uint8_t col = 1; col <= 8; ++col) {
                for (uint8_t row = 0; row < display_height; ++row) {
                    const uint8_t column = 
                    pixel_buffer[col * i - 1]
                }
            }
        }
    }

    // salva i driver passati i dsPass
    for (unsigned int d = 0; d < MAX_DRIVERS; d++) {
        if (ds[d]) {
            for (unsigned int dp = 0; dp < num_of_drivers; d++) {
                if (!dsPass[dp]) dsPass[dp] = ds[d];
            }
        }
    }

    
    
    for (unsigned int row; row < LED_CONTROL_REGISTERS / 2; row++) {
        for (unsigned int c = 0; c < num_of_drivers * 2; c+=2) {
            if (offset % 7 == 0) c++;
            uint8_t buffer = ds[d]->frames[ds[d]->picture_frame]->data[row];

            //ds[d]->frames[ds[d]->picture_frame]->data[row] = 
        }
    }
}

*/


IS31FL3731::~IS31FL3731() {
    for (int i = 0; i < MAX_DRIVERS; ++i) {
        if (drivers[i] != nullptr) {
            for (int f = 0; f < MAX_FRAMES; ++f) {
                delete drivers[i]->frames[f];
                drivers[i]->frames[f] = nullptr;
            }
            delete drivers[i];
            drivers[i] = nullptr;  // Per sicurezza, impostiamo il puntatore a nullptr
        }
    }
}