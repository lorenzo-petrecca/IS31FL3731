
# IS31FL3731

A versatile and efficient library for controlling **IS31FL3731 LED matrix drivers** on Arduino-compatible platforms (AVR, ESP32 and similar).

This library is designed to support:

- Up to **4 drivers simultaneously**
- **Text rendering** using built-in font system
- **Buffered frame control** for advanced manipulation
- **Direct register access** for low-level usage
- **Scrolling text animations**

<br/>

#### Index
- [Installation](#installation)
- [Overview](#overview)
- [Hardware](#hardware)
- [Software architecture](#software-architecture)
- [Usage Modes](#usage-modes)
- [Text rendering system](#text-rendering-system)
- [Scrolling system](#scrolling-system)
- [Supported platforms](#supported-platforms)
- [Types](#types)
  - [Frame](#frame)
  - [Function](#function)
  - [Mode](#mode)
- [Struct](#struct)
  - [DriverInfo](#driverinfo)
- [Classes](#classes)
  - [MatrixFrame](#matrixframe)
- [Drivers](#drivers)
- [I2C Address](#i2c-address)
- [Font](#font)
  - [How to add and use another font?](#how-to-add-and-use-another-font)
  - [Font methods](#font-methods)
  - [Font constructor](#font-constructor)
- [Error table](#error-table)
- [API Reference](#api-reference)
  - [set\_address](#set_address)
  - [check\_connection](#check_connection)
  - [init](#init)
  - [setOperatingMode](#setoperatingmode)
  - [setOperatingMode\_g](#setoperatingmode_g)
  - [setStartingFrame](#setstartingframe)
  - [setStartingFrame\_g](#setstartingframe_g)
  - [setIntensity](#setintensity)
  - [setIntensity\_g](#setintensity_g)
  - [writeFrame](#writeframe)
  - [allocateFrame](#allocateframe)
  - [freeFrame](#freeframe)
  - [freeAllFrames](#freeallframes)
  - [setFramePixels](#setframepixels)
  - [write](#write)
  - [setFont](#setfont)
  - [display](#display)
  - [clear](#clear)
  - [scrollText](#scrolltext)

<br/>

## Installation

This library is designed for the **Arduino framework** and can be used in both **Arduino IDE** and **PlatformIO** environments.

### Arduino IDE

You can install the library manually:

1. Download the repository as a ZIP file from GitHub
2. Open **Arduino IDE**
3. Go to:

   `Sketch` → `Include Library` → `Add .ZIP Library...`

4. Select the downloaded ZIP file

After installation, the library examples should appear in the Arduino IDE examples menu.

Alternatively, you can install the library manually by copying the library folder into your local Arduino `libraries` directory.


### PlatformIO

You can use the library in PlatformIO in different ways.

#### Option 1 — Local development copy

If you are developing or testing the library locally, place it inside the project's `lib/` folder:

```txt
your-project/
├── lib/
│   └── IS31FL3731/
├── src/
└── platformio.ini
```


#### Option 2 — GitHub dependency

You can also reference the library directly from GitHub in your platformio.ini file:

**Stable version (recommended)**

```ini
lib_deps =
    https://github.com/lorenzo-petrecca/IS31FL3731.git#v1.0.0
```

This ensures a fixed and stable version of the library.


**Development version**

```ini
lib_deps =
    https://github.com/lorenzo-petrecca/IS31FL3731.git
```

This will use the latest version from the main branch and may include unstable changes.


### Requirements

This library depends on:

- Arduino.h
- Wire.h

It is intended for boards using the Arduino framework and an available I2C interface.



<br/>

---

<br/>

## Overview

This library aims to provide a simple and immediate control interface for the IS31FL3731 LED driver, allowing the management of multiple drivers simultaneously, up to a maximum of 4 drivers, through the i2c interface.
Each driver controls 144 LEDs, arranged in two 9x8 (9 rows and 8 columns) matrix in charlieplexing configuration.
This library uses the <i>Wire.h</i> library to handle i2c communication and hence its related dependencies.


##### Informations by IS31FL3731 datasheet
*  `The IS31FL3731 is a compact LED driver for 144 single LEDs. The device can be programmed via an I2C compatible interface. The IS31FL3731 offers two blocks each driving 72 LEDs with 1/9 cycle rate. The required lines to drive all 144 LEDs are reduced to 18 by using the cross-plexing feature optimizing space on the PCB. Additionally each of the 144 LEDs can be dimmed individually with 8-bit allowing 256 steps of linear dimming.`
* `To reduce CPU usage up to 8 frames can be stored with individual time delays between frames to play small animations automatically. LED frames can be modulated with audio signal.`


So this driver can manage up to 8 frame registers without requiring the controller’s memory to store matrices. However, this library saves matrices in memory to allow for greater control over the device. It is also possible to clear the matrices or delete their instances to preserve memory, giving users of the library the flexibility to decide when to free up memory. Therefore, a large amount of memory is not necessary on the controller to use this library. We also reserve the possibility of updating the library to improve memory efficiency.

<br/>

---

<br/>

## Hardware

This library was primarily developed and tested on a custom dual-driver module named **Bee-two**, based on two IS31FL3731 drivers and a compact LED matrix.

Bee-two is not required to use this library, but it represents a reference implementation used during development.

More details about ***Bee-two module*** may be published in a dedicated repository in the future.


### Basic Requirements

To use this library, your hardware setup must provide:

- One or more IS31FL3731 drivers
- I2C connection (SDA, SCL)
- Proper power supply (VCC, GND)

Make sure that:
- SDA and SCL lines have pull-up resistors
- Each driver has a unique I2C address
- The power supply is stable and sufficient for the LED matrix (generally, both the drivers and the Bee-two module work well with either a 3.3 V or a 5 V power supply)


### Shutdown Pin (SDB)

Both the IS31FL3731 driver and the Bee-two module includes a shutdown pin (`SDB`) that must be handled correctly.

- If `SDB` is LOW → power off
- If `SDB` is HIGH → power on

You have two options:

- Connect `SDB` directly to VCC (always enabled)
- Control it via a GPIO pin for power management


### Development Setup

This library was primarily developed and tested on a custom module featuring (**Bee-two**):

- 2 × IS31FL3731 drivers
- LED matrix based on 0402 LEDs
- I2C communication interface

Because of this, most examples in this repository use a **dual-driver configuration**.


### Notes on Custom Hardware

You do NOT need the original module to use this library.
Any circuit using the IS31FL3731 with correct wiring will work.

However, when designing your own hardware, pay attention to:
- LED matrix layout (rows/columns mapping)
- Current limitations
- Proper decoupling capacitors
- I2C signal integrity

<br/>

---

<br/>

## Software architecture

The library is structured in three logical layers:

### 1. Low-level (I2C communication)

Direct communication with the IS31FL3731 chip:

- `selectPage()`
- `writeRegister()`
- `i2c_selectAndWrite()`

These functions handle raw communication with the device.


### 2. Driver management layer

Each physical IS31FL3731 chip is represented by a `DriverInfo` structure.

Main features:

- Driver initialization (`init`)
- Mode configuration (`setOperatingMode`)
- Frame selection (`setStartingFrame`)
- Intensity control (`setIntensity`)


### 3. Frame buffer layer (RAM-based control)

This is the **core of the library**.

Each driver can store up to 8 frames in memory:

- `allocateFrame()` → allocate memory
- `setFramePixels()` → modify frame content
- `clearFramePixels()` → clear RAM buffer
- `display()` → send frame to hardware

This allows:

- Full control before rendering
- Animations
- Text composition

<br/>

---

<br/>

## Usage modes

The library supports **two different usage approaches**.

### 1. Direct Mode (no RAM usage)

Write directly to the device registers without using memory:

```cpp
writeFrame(...)
```

|Pros|Cons|
|:--|:--|
|✅ No RAM usage|❌ No control after writing|
|✅ Fast|❌ No animation support|
|✅ Good for testing or static patterns||


### 2. Buffered Mode (recommended)

Work with frames in memory, then display them:

```cpp
allocateFrame(...)
write(...)
display(...)
```

|Pros|Cons|
|:--|:--|
|✅ Full control of content|❌ Uses RAM|
|✅ Enables animations||
|✅ Supports text rendering and scrolling systems||

> [!WARNING]
> ***Frame allocation is required***
> Before writing:
> ```cpp
> allocateFrame(driver, FRAME_1);
> ```
> Otherwise:
> - `write()` will not work correctly
> - `display()` may fail silently

> [!NOTE]
> `display()` ***does not allocate frames***, it only sends data already present in memory.


<br/>

---

<br/>

## Text Rendering System

The library includes a **font engine**:

- Characters are mapped via Font class
- Supports:
  - Numbers
  - Uppercase letters
  - Lowercase letters
  - Symbols

Each character is:

- 8 bytes tall
- Rendered into matrix rows

For more informations see [Font section](#font).
<br/>

---

<br/>

## Scrolling System

`scrollText()` implements a **non-blocking scrolling mechanism**:

- Uses millis() internally
- Must be called repeatedly inside loop()
- Automatically updates display

> [!IMPORTANT]
> This function is non-blocking and stateful.

Example:

```cpp
void loop() {
  matrix.scrollText("Hello World", 300, d1, d2);
}
```

> [!TIP]
> Return value:
> - **true** → scroll completed
> - **false** → scroll in progress

<br/>

---

<br/>

## Supported Platforms

The library is currently developed around the Arduino framework and depends on `Arduino.h` and `Wire.h`.  
The font access logic already distinguishes between AVR (`PROGMEM`) and ESP32 (direct access), so these are the main target families right now.

| Platform | Status | Notes |
|:--|:-:|:--|
| ESP32 | 🟢 ||
| AVR | 🟢 | Uses `PROGMEM` access in font system |
| Other Arduino-compatible boards with `Wire` | 🟡 | Not fully tested yet |


|Status|Description|
|:-:|:--|
|🟢|Compatible|
|🟡|Likely compatible (not tested)|
|🔴|Not compatible|

<br/>

---

<br/>


## Types
* [Frame](#frame)
* [Function](#function)
* [Mode](#mode)

<br/>

### Frame
Frame type represents the IS31FL3731 frame registers. 


Primitive type: ***uint8_t***

<br/>

**Enumeration table:**
| Name | Value |
|:-- |:-: |
| `FRAME_1` | `0x00` |
| `FRAME_2` | `0x01` |
| `FRAME_3` | `0x02` |
| `FRAME_4` | `0x03` |
| `FRAME_5` | `0x04` |
| `FRAME_6` | `0x05` |
| `FRAME_7` | `0x06` |
| `FRAME_8` | `0x07` |
| `NO_SELECTION` | `0xFF` |


<br/>
<br/>

### Function
Function type represents the function register and their enumeration represents all function available in function register (***0x0B***).
*The following table shows the values ​​of the "Function" enumeration and explains their respective functionality and the methods that use these register functions.*


> Primitive type: ***uint8_t***

<br/>

**Enumeration table:**
| Name | Value | Description | Methods |
|:-- |:-: |:-- |:-: |
| `CONFIG_MODE` | `0x00` | Configure the operation mode |  [[setOperatingMode](#setoperatingmode)] <br/> [[setOperatingMode_g](#setoperatingmode_g)] |
| `PICTURE_FRAME` | `0x01` | Set the display frame in Picture Mode | [[setStartingFrame](#setstartingframe)] <br/> [[setStartingFrame_g](#setstartingframe_g)] |
| `AUTO_PLAY_1` | `0x02` | Set the way of display in Auto Frame Play Mode |
| `AUTO_PLAY_2` | `0x03` | Set the delay time in Auto Frame Play Mode |
| `DISPLAY_OPTION` | `0x05` | Set the display option |
| `AUDIO_SYNC` | `0x06` | Set audio synchronization function |
| `FRAME_STATE` | `0x07` | Store the frame display information |
| `BREATH_CTRL_1` | `0x08` | Set fade in and fade out time for breath function |
| `BREATH_CTRL_2` | `0x09` | Set the breath function |
| `SHUTDOWN` | `0x0A` | Set software shutdown mode |
| `AGC` | `0x0B` | Set the AGC function and the audio gain |
| `ADC` | `0x0C` | Set the ADC sample rate of the input signal |

<br/>

> [!WARNING]
> Not all functions displayed in the table have a use in this library version 

> [!NOTE]
> This library at the moment does not support many features such as all audio functions, and some auto-play settings

<br/>
<br/>

### Mode
Mode type represent the config register. This enumeration represents the mode of the IS31FL3731 driver. The default mode is *PICTURE_MODE*.

> Primitive type: ***uint8_t***

<br/>

**Enumeration table:**
| Name | Value |
|:-- |:-: |
| `PICTURE_MODE` | `0x00` |
| `AUTO_FRAME_PLAY_MODE` | `0x08` |
| `AUDIO_FRAME_PLAY_MODE` | `0x10` |

<br/>

> [!WARNING]
> 🚧👷🔧 
> working on features:
>  - auto play mode
>  - audio play mode

<br/>

---

<br/>

## Struct

* [DriverInfo](#driverinfo)

### DriverInfo
The DriverInfo struct is the main structure of this library. It represents a single driver.

| Field id. | Description | Type | Default |
|--- |--- |--- |--- |
| address | I2C device address | uint8_t | 
| operating_mode | IS31FL3731 operating mode | [Mode](#mode) | PICTURE_MODE |
| picture_frame | Frame to start writing from | [Frame](#frame) | FRAME_1 | 
| *frames[MAX_FRAMES] | Array of frame pointers | [MatrixFrame](#matrixframe) | *nullptr* for all frames |

<br/>

````cpp
// DriverInfo constructor
DriverInfo (uint8_t addr, Mode mode, Frame frame)
````
<br/>

---

<br/>

## Classes

* [MatrixFrame](#matrixframe)

### MatrixFrame
This class represents a single register frame for a device, consisting of a static 9x2 matrix.
MatrixFrame class only has public variables and methods.
The class consists of only one array ( <u style="font-weight:500; font-style:italic;">uint8_t <span>data<span>[LED_CONTROL_REGISTERS]</u> ), where *LED_CONTROL_REGISTERS* represents the number of registers in the frame, which is set to 18 by default, since there are 18 registers in the led control register of the IS31FL3731 device, 9 per matrix.
This class also provides some methods:

| Name | Description | Parameters | Output |
|--- |--- |--- |---|
| clear | Clear the data array, bringing back all bytes to 0x00 | | |
| setRow | set a single row in matrix frame to a specific value | uint8_t row, uint8_t value | (bool) true if the data array was successfully written

> [!WARNING]
> Instances of the **MatrixFrame** class are manually created by the library directly within the specific array (`MatrixFrame *frames[MAX_FRAMES]`), a field of the IS31FL3731 driver instance, using the *<u>allocateFrame()</u>* function. They are then manually deleted and freed from memory as needed by other library functions, such as *<u>freeFrame()</u>*.
>

<br/>

---

<br/>

## Drivers
As mentioned, this library can use between one and four IS31FL3731 devices simultaneously. To use them, they must be instantiated using the constructor of the previously discussed [DriverInfo](#driverinfo) struct.
The library handles both the driver instances and the memory they occupy. If any of the four available drivers is not instantiated, it will be ignored during operations, and an error (typically with code -1) will be returned if access is attempted.
The instantiated drivers are stored in the private *drivers variable, an array of pointers to the DriverInfo type. The library does not use a specific constructor for the main IS31FL3731 class, so when an instance is created, no parameters need to be passed, and the pointers to DriverInfo instances will initially point to nothing.
The actual initialization will occur with the [init()](#init) method. Only at this point, by passing the addresses of the drivers to be instantiated to the function, will the driver instances be created and their respective pointers inserted into the array.

<br/>

---

<br/>

## I2C Address
The drivers must be initialized with the [init](#init) method by passing the addresses of drivers that must be initialized.

> [!WARNING]
> Only drivers whose address was passed to the function will be initialized and available.

<br/>

Drivers can be assigned on of the following 4 addresses, depending how the AD pin of the IS31FL3731 device is connected:


- **0x74** : AD pin connected to GND
- **0x77** : AD pin connected to VCC
- **0x75** : AD pin connected to SCL
- **0x76** : AD pin connected to SDA



> [!CAUTION]
> Other addresses may not be recognized by the driver.


<br/>

---

<br/>

## Font
The font in this library is represented by a class. It is not necessary to use the font to manage the IS31FL3731 driver with this library. However, this class is essential if you want to use the library’s [write](#write) method to automatically display strings on the screen without manually setting the registers. The actual font within the Font class is stored in a variable of type [FontType](#fonttype). This approach allows for the use of custom fonts when needed. The library’s default font is "font8x5", saved in the "font8x5.h" header file of this library. In the following sections, we will see how to use a custom font and the methods of this class.

#### How to add and use another font?
To use a different font, you need to add a header file to the library, which you can name as you prefer. However, the file must be structured in a specific way to function properly. Below is a brief example. You can also refer to the existing "font8x5.h" file to correctly build the new font.
At this point, to use the new font, you simply need to set it with the [setFont](#setfont) method.

```` cpp
/* yourFont.h */

#ifndef YOUR_FONT_H
#define YOUR_FONT_H

#include "Arduino.h"
#include "FontType.h"

FONT_STORAGE FontType yourFont {
        {   // 0
            B00111000,
            B01000100,
            B01001100,
            B01010100,
            B01100100,
            B01000100,
            B00111000,
            B00000000
        },
        {   // 1
            B00010000,
            B00110000,
            B00010000,
            B00010000,
            B00010000,
            B00010000,
            B00111000,
            B00000000
        },
        // other characters ...
};

#endif
````

> [!WARNING]
> The characters specified in the FontType constant must match the number of characters in the FontType structure; otherwise, the library will not work. Additionally, to ensure the font functions correctly, you should follow the character sequence in the FontType structure, which you can find in the "FontType.h" header file. 

<br/>

#### Font methods
| Name | Description | Parameters | Output |
|--- |--- |--- |--- |
| `getCharacter` | gets the pointer to the first element of the specified character data array | `char c` | (`uint8_t*`)
| `getCurrentFont*`  | gets the setted font structure | | `FontType` |
| `setCurrentFont` | sets the current font | `FontType font` | |

> [!CAUTION]
> `getCurrentFont` method is only available on ESP32 

<br/>

#### Font constructor

````cpp
Font(const FontType *font) : current_font(font)
````
<br/>

---

<br/>

## Error table
The following table shows the error codes and their respective meanings. It is important to note that only the methods of the class that reference the error table use the codes listed below and their corresponding descriptions.

| Err. Code | Description |
| ---: | --- |
| **-1** | Driver is not initialized |
| **0** | Succes |
| **1** | Length to long for buffer |
| **2** | Address send, NACK received |
| **3** | Data send, NACK received |
| **4** | Other error (lost bus arbitration, bus error, ..) |
| **5** | Timeout |

<br/>

---

<br/>

## API Reference

### `set_address`
This method can accept between 1 to 4 i2c addresses as function arguments, represented by variables of type uint8_t. These indicate the values to which the address of the respective driver instance, saved in the DriverInfo *drivers array, will be set.
````cpp
void set_address (uint8_t *addr1, uint8_t *addr2 = nullptr, uint8_t *addr3 = nullptr, uint8_t *addr4 = nullptr);
````


> [!IMPORTANT]
> - The parameters are pointers, so the memory address of the variable must be passed
> - This method should only be called after initializing the driver(s) to modify their i2c address
> - If you wish to use the default addresses, there is no need to ever call this method

<br/>

### `check_connection`
This method checks if the i2c connection is established.
The check can be done by passing either the i2c address directly to the function or by passing the DriverInfo structure representing the driver whose connection you want to verify.
````cpp
uint8_t check_connection (uint8_t addr);
uint8_t check_connection (DriverInfo driver);
````

> [!TIP]
> This method return 0 if true, otherwise return the Wire library error code ([Error table](#error-table))

> [!IMPORTANT]
> This function must be called only after the init function has been called.

</br>

### `init`
This method is used to initialize the library’s drivers. It acts as a constructor for all the drivers you wish to initialize. The method can be called in two different ways: in one approach, it accepts as parameters, similar to the set-address function, the i2c addresses of the respective drivers (only the drivers for which an address was provided will be initialized, up to 4); the other approach accepts as a parameter the number of drivers, from 1 to 4, that you want to initialize, and in this case, the i2c addresses will be assigned automatically as described in the following table.

Additionally, this method sets each initialized driver to the default operating mode, **PICTURE_MODE**, and the starting frame to **FRAME_1**.

| Driver number | Default i2c address |
|--- |--- |
| 1 | 0x74 |
| 2 | 0x77 |
| 3 | 0x75 |
| 4 | 0x76 |

````cpp
uint8_t init (uint8_t addr1 = 0xff, uint8_t addr2 = 0xff, uint8_t addr3 = 0xff, uint8_t addr4 = 0xff);
uint8_t init (uint8_t number_of_drivers);
````

> [!NOTE]
> The init method returns the number of successfully initialized drivers as a ***uint8_t*** data type.

<br/>

### `setOperatingMode`
This method sets the driver’s operating mode. The available operating modes for the driver are defined in the table in the [Mode](#mode) section. The method accepts as parameters the driver on which to set the operating mode, the operating mode, and the frame.

> [!NOTE]
> It is necessary to pass the frame to the method only if you intend to use the **AUTO_PLAY_MODE**.

<br/>

````cpp
int setOperatingMode (DriverInfo *driver, Mode mode, Frame frame = FRAME_1);
````


> [!TIP]
> The function returns the status of the operation as an int data type:
- **-1**: driver is not initialized
- **other values**: [Error table](#error-table)
</br>

### `setOperatingMode_g`
This method has the same purpose as the setOperatingMode method, with the difference that it allows setting a single operating mode for all drivers in a single call.
It accepts the operating mode and the frame as parameters (see the note in [setOperatingMode](#setoperatingmode) for information on the frame). Unlike setOperatingMode, this method is of type void.

````cpp
void setOperatingMode_g (Mode mode, Frame frame = FRAME_1);
````
<br/>

### `setStartingFrame`
This method sets the starting frame for the specified driver. This setting is only useful if you are using the **PICTURE_MODE** operating mode. Setting the frame is important to define from which frame the driver should begin reading data.

````cpp
int setStartingFrame (DriverInfo *driver, Frame frame);
````

> [!TIP]
> The function returns the status of the operation as an int data type:
> **-1**: driver is not initialized</li>
> **other values**: [Error table](#error-table)

<br/>

### `setStartingFrame_g`
This method serves the same function as [setStartingFrame](#setStartingFrame), with the only differences being that this method does not return anything and sets the same frame for all initialized drivers. Therefore, it does not require any DriverInfo instance.

````cpp
void setStartingFrame_g (Frame frame);
````
<br/>

### `setIntensity`
This method is used to set the brightness of the LEDs using PWM. It can be called in two different ways. One approach allows setting the intensity of a single LED by passing the corresponding **pwm_register** as a parameter. A different approach omits the pwm_register, which results in the same PWM intensity being set for all LEDs, saving the same value in all pwm_registers.

````cpp
int setIntensity (DriverInfo *driver, uint8_t pwm_register, unsigned int intensity, Frame frame = FRAME_1);
int setIntensity (DriverInfo *driver, unsigned int intensity, Frame frame = FRAME_1);
````

> [!TIP]
> The function returns the status of the operation as an int data type:
> **-1**: driver is not initialized
> **other values**: [Error table](#error-table)


> [!NOTE]
> - The intensity value must be within the range **0 ~ 100**
> - If the ***frame*** parameter is omitted, the default **FRAME_1** will be set. 

<br/>

### `setIntensity_g`
Like the [setIntensity](#setintensity) method, this is used to manage the PWM intensity of the LEDs. However, this method can only set the same value for all LEDs at once, and it does so for each initialized driver. Otherwise, it works in the same way as the setIntensity method but does not return anything.

````cpp
void setIntensity_g (unsigned int intensity, Frame frame = FRAME_1);
````
<br/>

### `writeFrame`
This function allows writing to the registers of the enabled drivers. It takes as input parameters the frame to write to (the same for all drivers), which defaults to **FRAME_1** if omitted, and a two-dimensional array with the following dimensions:
* **MAX_DRIVERS**: The maximum number of drivers supported by the library, typically set to **4** by default.
* **LED_CONTROL_REGISTERS**: The maximum number of registers for each driver, typically set to **18** by default.
The values to be inserted into the array are the values to assign to the registers, with **one byte per register**, so that each register can control 8 LEDs.

````cpp
bool writeFrame (uint8_t ledRegisters[MAX_DRIVERS][LED_CONTROL_REGISTERS], uint8_t frame = FRAME_1);
````

> [!TIP]
> The function returns true if all communication succes, return false otherwise.


> [!IMPORTANT]
> This function should only be called for testing purposes, as it does not save data to the driver's frame array but writes directly to the driver's frame register to display the sent bytes. If you want more control over the driver using this library, it is recommended to use other library methods that allocate memory for your byte arrays. However, if you aim to preserve controller memory, this function can be useful.

<br/>

### `allocateFrame`
This method allocates memory space for the data matrix of the specified frame and driver. It creates an instance of [MatrixFrame](#matrixframe). This approach is useful for gaining greater control over individual LEDs and the driver, at the expense of the controller's memory. It is not strictly necessary to use this part of the library to communicate with the driver, but it is recommended if you want to fully utilize the library and do not need to prioritize saving memory space on the controller, as this approach increases memory usage.

````cpp
bool allocateFrame (DriverInfo *driver, Frame frame);
````

> [!NOTE]
> The function returns **true** if the memory for the frame in the current driver instance has been successfully allocated; otherwise, it returns **false**.


> [!WARNING]
> This function exclusively allocates memory for the frame in the current driver instance. It neither sends data to the driver nor writes data into the instance. However, it is necessary to deallocate the memory when it is no longer needed.

<br/>

### `freeFrame`
Frees the memory of the [MatrixFrame](#matrixframe) instance associated with the specified driver. This method is used to deallocate the memory previously allocated with the [allocateFrame](#allocateframe) method.

````cpp
bool freeFrame (DriverInfo *driver, Frame frame);
````

> [!NOTE]
> The function returns **true** if the memory has been successfully deallocated; otherwise, it returns **false**.

<br/>

### `freeAllFrames`
This method frees the memory associated with all frames (all [MatrixFrame](#matrixframe) instances) of the specified driver.

````cpp
bool freeAllFrames (DriverInfo *driver);
````

> [!NOTE]
>The function returns **true** if the memory has been successfully deallocated; otherwise, it returns **false**.

<br/>

### `setFramePixels`
This method is responsible for filling the frame matrices with the data to be sent to the driver. It sets the data in the specific frame of the [MatrixFrame](#matrixframe) instance of the specified driver.

````cpp
bool setFramePixels (DriverInfo *driver, Frame frame, uint8_t *data);
````

> [!NOTE]
>Success **true**, **false** otherwise.


> [!IMPORTANT]
>This function only stores the data in the controller's memory; it does not send any data to the driver.

<br/>

### `write`
This method, unlike ***`setFramePixels`***, does not manually fill the frame matrix with the provided data but instead performs automatic assignments based on the characters passed as arguments to the function. It accepts a string as input and automatically distributes the characters across the available drivers based on the font used.
If no driver is specified, `write()` distributes characters across initialized drivers.
Like ***`setFramePixels`***, this method does not write to the driver and only saves the data in memory, but writes it to the previously set current frame. Therefore, it is necessary to call the methods [setStartingFrame](#setstartingframe) or [setStartingFrame_g](#setstartingframe_g) before using this method.

````cpp
void write (const char *str, DriverInfo *driver, bool highlight)
````

> [!WARNING]
> The method takes into account the font size, character length, and the number of initialized drivers. *Currently, the library only supports managing **2 characters per driver***, so if the input string exceeds the total number of characters available on the module, they will not be displayed. Future updates to the library may introduce methods to better handle such situations. For now, a workaround is to manually manage the matrix data and operate directly on individual frames to alternate their display, though this approach forfeits the ability to use fonts.

> [!NOTE]
> Current behavior is primarily tested on 1–2 driver setups.

<br/>

### `setFont`
This method sets the font for the characters displayed on the screen. It is the font used by the controller to send data to the drivers, and it will be the same for all drivers. For more information about the font and how it is managed by this library, refer to the [Font](#font) section.

````cpp
void setFont(FontType *font);
````

<br/>

### `display`
This method is responsible for displaying on the screen of the module based on IS31FL3731 what has been saved in the frame registers. This means that, via I2C communication, a command will be sent to the specified driver to display the current frame (***picture_frame***), , which has been previously set using [setStartingFrame](#setstartingframe) or [setStartingFrame_g](#setstartingframe_g).
It is important to note that if you want to interact with all initialized drivers, you can simply omit any arguments when calling the function. Otherwise, if you want to interact with more than one but not all initialized drivers, you need to call the method multiple times, passing each driver individually.

````cpp
int display (DriverInfo *driver = nullptr);
````

> [!NOTE]
> The function returns the number of drivers that were successfully written.

<br/>

### `clear`
This method clears the selected frame by turning off all LEDs.
If no frame is specified, all frames are cleared; similarly, if no driver is specified, the action is performed on all instantiated drivers.

````cpp
void clear (Frame frame = NO_SELECTION, DriverInfo *driver = nullptr);
````

<br/>


### `scrollText`
The purpose of this method is to allow a string longer than the available screen space to be displayed. This function scrolls the text across the display based on the number of drivers that have been successfully initialized and passed as parameters (max. 4).
The text will scroll from right to left, that is, from the last driver passed to the first. The text scrolls by a fixed offset of one character. If only one driver is passed, the text will always scroll one character at a time across the single driver (each driver supports two characters).

````cpp
bool scrollText (const char *str, unsigned long interval, DriverInfo *driver_1 = nullptr, DriverInfo *driver_2 = nullptr, DriverInfo *driver_3 = nullptr, DriverInfo *driver_4 = nullptr);
````

> [!WARNING]
> You must have correctly initialized the drivers you wish to use to scroll the text across the matrices of the various drivers.


<br/>