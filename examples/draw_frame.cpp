#include <Arduino.h>
#include "IS31FL3731.hpp"

IS31FL3731 screen;

#define ADDR_1 0x77
#define ADDR_2 0x74

#define SMILE \
  { \
    B00000111,  \
    B00011000,  \
    B01100000,  \
    B10000100,  \
    B10000000,  \
    B10000100,  \
    B01100011,  \
    B00011000,  \
    B00000111,  \
    \
    B11100000,  \
    B00011000,  \
    B00000110,  \
    B00100001,  \
    B00000001,  \
    B00100001,  \
    B11000110,  \
    B00011000,  \
    B11100000   \
  }

#define SAD \
  { \
    B00000111,  \    
    B00011000,  \
    B01100000,  \
    B10000110,  \
    B10000000,  \
    B10000001,  \
    B01100010,  \
    B00011000,  \
    B00000111,  \
    \
    B11100000,  \
    B00011000,  \
    B00000110,  \
    B01100001,  \
    B00000001,  \
    B10000001,  \
    B01000110,  \
    B00011000,  \
    B11100000   \
  }


uint8_t test_frame[][LED_CONTROL_REGISTERS] = {SMILE, SAD};

void setup() {
  // Inizializza due driver, il primo con indirizzo 0x77, il secondo 0x74
  // Initialize drivers. First driver address is 0x77, second 0x74
  screen.init(ADDR_1, ADDR_2);
  Serial.begin(115200);
  delay(1000);

  // Controlla se la connessione con i due driver è stabilita correttamente
  // Check driver connection
  int check1 = -1, check2 = -1;
  while (check1 != 0 && check2 != 0) {
    check1 = screen.check_connection(ADDR_1);
    check2 = screen.check_connection(ADDR_2);
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
  
  // select operating mode (all drivers)
  screen.setOperatingMode_g(PICTURE_MODE);  // Seleziona la modalità operativa
  // select starting frame (all drivers)
  screen.setStartingFrame_g(FRAME_1); // seleziona il frame di partenza
  // set the screen light intensity (all drivers)
  screen.setIntensity_g(50, FRAME_1); // setta l'intensità luminosa dello schermo

  delay(350);

  screen.writeFrame(test_frame, FRAME_1);
}

void loop() {
  
}
