# 🦻 OpenEarable - Firmware v1.4.0

OpenEarable is a new, open-source, Arduino-based platform for ear-based sensing applications. It provides a versatile prototyping platform with support for various sensors and actuators, making it suitable for earable research and development.

<kbd> <br> [Get OpenEarable device now!](https://forms.gle/R3LMcqtyKwVH7PZB9) <br> </kbd>
<p align="center" width="100%" style="max-width: 300px">
 <img width="400" src="https://github.com/OpenEarable/open-earable/assets/11386075/e24b08eb-ecaa-4790-9d08-2e192c11fb59">
</p>



## Table of Contents
- [Introduction](#Introduction)
- [Setup](#Setup)
  - [Arduino IDE](#Arduino-IDE) 
  - [Arduino Libraries](#Arduino-Libraries)
  - [SD Card Setup](#SD-Card-Setup)
  - [SPI Setup](#SPI-Setup)
  - [sdfat Library Setup](#sdfat-Library-Setup)
  - [BMP280 Library Setup](#BMP280-Library-Setup)
- [Usage](#Usage)
  - [Install OpenEarable](#Install-OpenEarable)
  - [Default Firmware](#Default-Firmware)
  - [Dashboard](#Dashboard)
  - [edge-ml](#edge-ml)
- [BLE Specification](#BLE-Specification)
- [Firmware Internals](#Firmware-Internals)
  - [Bootloader](#Bootloader)
  - [Pin Configuration](#Pin-Configuration)
  - [Earable](#Earable)
  - [Sensor Configuration](#Sensor-Configuration)
  - [Button](#Button)
  - [LED](#LED)
- [Cite](#Cite)

## Introduction

OpenEarable is designed to enable ear-based sensing applications by offering a flexible and open-source hardware platform. It incorporates a range of sensors, including a 9-axis inertial measurement unit, an ear canal pressure and temperature sensor, an inward-facing ultrasound microphone, a speaker, a push button, and a controllable RGB LED. With these features, OpenEarable provides researchers and developers with the ability to explore various application scenarios. 
For more information visit the [OpenEarable](https://open-earable.teco.edu/) website.

OpenEarable is controlled and streams sensor data via BLE (Bluetooth Low Energy). Audio is played from and recorded to the internal SD card (required card SanDisk Extreme Class 3, must be formatted as exFAT). OpenEarable is compatible with the provided [dashboard](https://github.com/OpenEarable/dashboard) and [edge-ml](https://edge-ml.org/). 


## Setup

### Arduino IDE
Download and install the Arduino IDE. OpenEarable is based on the "Arduino Nano 33 BLE Sense" board. Therefore, you first have to install the required dependencies ("Arduino Mbed OS Nano Boards" via boards manager) in your Arduino IDE following this [Setup Guide](https://docs.arduino.cc/software/ide-v2/tutorials/ide-v2-board-manager#mbed-os-nano).

### Arduino Libraries
The following Arduino Libraries have to be installed in your Arduino IDE by navigating to `Sketch -> Include Library -> Manage Libraries`:
- [EdgeML-Arduino (version 1.3.3)](https://github.com/edge-ml/EdgeML-Arduino), which includes the following dependencies that are also required and automatically installed:
	- [ArduinoBLE](https://github.com/arduino-libraries/ArduinoBLE)
- [Adafruit_BMP280](https://github.com/adafruit/Adafruit_BMP280_Library)
- [DFRobot_BMX160](https://github.com/DFRobot/DFRobot_BMX160)
- [SdFat - Adafruit Fork](https://github.com/adafruit/SdFat)


### SD Card Setup
In order to be compatible with the OpenEarable library the SD card needs to be formatted with the exFAT format.
Make sure to have a sufficiently fast SD card. (Required SD Card: SandDisk class 10 and class A30)

### SPI, Wire, and Variant Setup

The default Arduino implementation of the SPI library does not meet the required speed. To address this, optimized SPI files are provided. Follow the steps below to integrate these files into Arduino.
All referenced files can be found in the "resources" folder in the "spi_files" subfolder.

To fully integrate the optimized SPI files, changes to the Arduino Nano 33 BLE board files have to be made. Follow the steps below to accomplish this:

1. Navigate to the Arduino15 folder as described in [this tutorial](https://support.arduino.cc/hc/en-us/articles/4415103213714-Find-sketches-libraries-board-cores-and-other-files-on-your-computer#boards).

2. Navigate to the following directory: `packages/arduino/hardware/mbed_nano/4.0.4/libraries`. *Please note:* If you have a different version than mbed_nano 4.0.4 installed or you don't have it installed at all, you can change or add it by navigating to `Tools -> Board -> Boards Manager`. Search for `Arduino Mbed OS Nano Boards` and install the desired version.

3. Locate the existing `SPI` library folder within this directory. It needs to be swapped with the provided folder named "SPI" in "resources/spi_files" found in this repository.

4. Locate the existing `Wire` library folder within the libraries folder. It needs to be swapped with the provided folder named "Wire" in "resources/wire_files" found in this repository (make sure to delete the Wire.cpp file).

5. Navigate to the following directory in the `Arduino15`directory: `packages/arduino/hardware/mbed_nano/4.0.4/cores/arduino`.

6. Place the files `RingBuffer.h` and `RingBuffer.cpp` from "resources/wire_files" into this folder.

7. Similarly, navigate to the following directory: `packages/arduino/hardware/mbed_nano/4.0.4/cores/arduino/mbed/targets/TARGET_NORDIC/TARGET_NRF5x/TARGET_SDK_15_0/modules/nrfx/drivers`.

8. In the subdirectory `include`, locate the file named `nrfx_spim.h` and replace it with the provided `nrfx_spim.h` file from the `resources/spi_files` folder of this repository.

9. In the subdirectory `src`, place the `nrfx_spim.c` file provided under `resources/spi_files` of this repository (note: you will not find an existing file named nrfx_spim.c).
    
10. Navigate back to the `Arduino15` folder. Navigate to `packages/arduino/hardware/mbed_nano/4.0.4/variants/ARDUINO_NANO33BLE`. Replace `pins_arduino.h` and `variant.cpp `with the files provided under `resources/variant` of this repository.

### sdFat Library Setup
One of the library dependencies is the SdFat library from Bill Greiman.
This library is used to send data to the SD card.
To achieve the desired write speeds of up to 1.5Mbps the library has to be modified slighlty.

1. Go to the `Arduino/libraries` folder, locate the `SdFat_-_Adafruit_Fork` library folder and open it. (Note: typically, the Arduino libraries folder is located at `~/Documents/Arduino/libraries`)
   
3. Inside the `src` folder, replace the `SdFatConfig.h` with the provided `SdFatConfig.h` file found in the `resources/sdfat_config` folder of this repository.

### BMP280 Library Setup
The BMP280 library has to be slightly modified.
1. Go to the `Arduino/libraries` folder (commonly found in your `Documents` folder) and locate the `Adafruit_BMP280_Library` folder.
2. Replace the files `Adafruit_BMP280.cpp` and `Adafruit_BMP280.h` with the files found in the `resources/Adafruit_BMP280_Library` folder of this repository.
   

### Reboot
To make sure that all your changes are applied correctly, restart your computer.

## Usage
### Install OpenEarable
Now that all dependencies are configured, the last step is to install this repository as a library as follows:

1. In your Arduino IDE click on `Sketch -> Include Library -> Add .ZIP Library....`
2. Navigate to the location where you saved the downloaded zip file.
3. Select the zip file and click Open.

Once done, the library should be available in the `Sketch -> Include Library` menu.

### Default Firmware
The easiest way to use OpenEarable is with the provided App sketch. 
It can be found within Arduino under `File -> Examples-> OpenEarable -> App`.


The absolute minimum needed to run the code successfully is the following:

```c++
#include "Arduino.h"
#include "OpenEarable.h"

void setup()
{
    open_earable.begin();
}

void loop()
{
    open_earable.update();
}
```

With this minimum sketch, all internal functionality is activated and OpenEarable becomes controllable via our [Dashboard](https://github.com/OpenEarable/dashboard), via [EdgeML](https://edge-ml.org/), and via the BLE API.

### Flashing
To flash the firmware, make sure you select `Arduino Nano 33 BLE` as target and the port that your OpenEarable is connected to. Then simply press the `Upload` arrow.

### Dashboard

OpenEarable can be directly connected to our [Dashboard](https://github.com/OpenEarable/dashboard).

### edge-ml

OpenEarable is seamlessly integrated with [edge-ml.org](https://edge-ml.org/), an open-source and browser-based toolchain for machine learning on microcontrollers.

### BLE Specification

The following table contains the BLE specifications with the available Services and Characteristics as well as UUIDs.
The first 3 Services are from the [EdgeML-Arduino](https://github.com/edge-ml/EdgeML-Arduino) library.

Note: to configure sensors via BLE see [Sensor Configuration](#Sensor-Configuration).


Specification Table:

| Service Name        | Service UUID                             | Characteristic Name  | Characteristic UUID                     | Permissions |
|---------------------|------------------------------------------|----------------------|-----------------------------------------|-------------|
| Sensor Service      | `34c2e3bb-34aa-11eb-adc1-0242ac120002`   | Sensor Configuration | `34c2e3bd-34aa-11eb-adc1-0242ac120002`  | Write       |
|                     |                                          | Sensor Data          | `34c2e3bc-34aa-11eb-adc1-0242ac120002`  | Read/Notify |
| Device Info Service | `45622510-6468-465a-b141-0b9b0f96b468`   | Device Identifier    | `45622511-6468-465a-b141-0b9b0f96b468`  | Read        |
|                     |                                          | Device Generation    | `45622512-6468-465a-b141-0b9b0f96b468`  | Read        |
| Parse Info Service  | `caa25cb7-7e1b-44f2-adc9-e8c06c9ced43`   | Scheme               | `caa25cb8-7e1b-44f2-adc9-e8c06c9ced43`  | Read        |
| Audio Service    | `5669146e-476d-11ee-be56-0242ac120002`   | Source             | `566916a8-476d-11ee-be56-0242ac120002`  | Read/Notify/Write  |
|   |   | State            | `566916a9-476d-11ee-be56-0242ac120002`  | Read/Notify/Write  |
| Battery Service     | `180F`                                   | Battery Level        | `2A19`                                  | Read/Notify |
| Button Service      | `29c10bdc-4773-11ee-be56-0242ac120002`   | Button State         | `29c10f38-4773-11ee-be56-0242ac120002`  | Read/Notify |
| LED Service         | `81040a2e-4819-11ee-be56-0242ac120002`   | LED Set State        | `81040e7a-4819-11ee-be56-0242ac120002`  | Write       |


#### Sensor Configuration Characteristic
Permissions: Write

This characteristic is used to send a sensor configuration to the Earable.

Configuration Package:

| Byte 0     | Byte 1-4    | Byte 5-8 |
|------------|-------------|----------|
| SensorID   | Sample Rate | Latency  |
| uint8      | float       | uint32   |

SensorID: ID of the sensor.<br>
Sample Rate: Desired sample rate. <br>
Latency: Legacy field which is mostly ignored. However, it has been repurposed as shown later.

Each sensor or audio IO can be enabled individually or together at the same time with predefined configurations.
It is recommended to use the predefined configurations.


To use the redefined configurations, Please refer to the [implementation](#CONFIGURATION) details below.

#### Sensor Data Characteristic
Permissions: Read/Notify

This Characteristic is responsible for sending data packages from the Earable to the connected device.

Data Package:

| Byte 0   | Byte 1 | Byte 1-4   | Byte 5-X   |
|----------|--------|------------|------------|
| SensorID | Size   | Time Stamp | Data Array |
| uint8    | uint8  | uint32     | ---        |


SensorID: ID of the sensor.<br>
Size: Length of the data array.<br>
Time Stamp:  Timestamp in milliseconds.<br>
Data Array: Array of bytes, which need to be parsed according the sensors parsing scheme.

#### Device Identifier Characteristic
Permissions: Read

This characteristic is used to get the Device Identifier string.

#### Device Generation Characteristic
Permissions: Read

This characteristic is used to get the Device Generation string.

#### Scheme Characteristic
Permissions: Read

With this characteristic the parsing scheme information can be requested from the device.
The parsing scheme is needed to convert a received data package to usable values.
More information about parsing the scheme can be found in the [EdgeML-Arduino](https://github.com/edge-ml/EdgeML-Arduino) library.

#### Audio Play Service
The Audio Player service consists of two characteristics. One to set the audio source and one two controll the playing state of the player. To properly set a new source.

#### Source Characteristic
Permissions: Read/Write

This characteristic is used to set the source of the audio player. Writing to this characteristic will result in stopping the player and playing from the new source.

The first byte of the characteristic indicates the source type being:
- 0: Idle
- 1: Mono WAV File from SD Card
- 2: Single Tone between 300-22000 Hz
- 3: Jingle (chosen from a list of predefined sounds)

The meaning of the remaining remaining bytes depends on the source type and is given for each type below.

Play Package:

##### Idle
| Byte 0 | Byte 1    | Byte 2-X     |
|--------|-----------|--------------|
| 0      | -         | -            |
| uint8  | uint8     | char array   |

##### WAV File
| Byte 0 | Byte 1      | Byte 2-X     |
|--------|-------------|--------------|
| 1      | name length | file name    |
| uint8  | uint8       | char array   |

The file on the SD Card has to be in the format:
- File type: WAV
- Format tag: PCM
- Number of Channels: 1 (Mono)
- Sample rate: 44100
- Bits per Sample: 16

##### Constant Tone
| Byte 0 | Byte 1    | Byte 2-6     | Byte 7-11        |
|--------|-----------|--------------|------------------|
| 2      | Waveform  | frequency    | amplitude [0, 1] |
| uint8  | uint8     | float        | float            |

Available Waveforms are:
- 0: Sine
- 1: Square
- 2: Triangle
- 3: Saw

##### Jingle Player
| Byte 0 | Byte 1    | Byte 2-X     |
|--------|-----------|--------------|
| 3      | ID        | -            |
| uint8  | uint8     | char array   |

Jingle IDs:
- 1: Notification
- 2: Success
- 3: Error
- 4: Alarm
- 5: Ping
- 6: Open 
- 7: Close
- 8: Click

#### State Characteristic
Permissions: Read/Write/Notify
Sets the state of the player.

State (uint8):
- 0: Undefined (Idle)
- 1: Play
- 2: Pause
- 3: Unpause

#### Battery Level Characteristic
Permissions: Read/Notify

Read the current battery level. The read value is a 1 byte int from 0-100 representing battery charge in percent.

#### Battery State Characteristic
Permissions: Read/Notify

Read the current charging state:

The read value is a 1 byte uint8
- 0: battery
- 1: charging
- 2: fully charged

#### Button State Characteristic
Permissions: Read/Notify

Sends the state of the button as a 1 byte int.
A notification is sent once a change of button state has occurred.

The states are:
- 0: RELEASED
- 1: PRESSED

#### LED Set State
Permissions: Read/Write

Set LED state as 3 byte RGB value.

| Byte 0    | Byte 1      | Byte 2-X     |
|-----------|-------------|--------------|
| Red value | Green value | Blue value   |
| uint8     | uint8       | uint8        |

## Firmware Internals

### Bootloader

OpenEarable is based on the Arduino Nano 33 BLE and uses the respective [bootloader](https://github.com/arduino/ArduinoCore-mbed/tree/main/bootloaders/nano33ble) with Mbed OS. 
Under "resources/Arduino_Nano_33_bootloader" the bootloader binary can be found as well.

### Pin Configuration

A handy header file with all the pin definition is provided. For reference look at the "Earable_Pins.h".

## Earable

The easiest way to use edge-ml is with the provided `App` sketch.

However, there are a few more functionalities.

Note: The majority of the following functions should be called BEFORE the `edge_ml_earable.begin()` function.

#### `void debug(Stream &stream)`

Set a IO stream for debugging purposes.

```c++
edge_ml_earable.debug(Serial);
```

#### `void configure_sensor(SensorConfigurationPacket& config)`

Send a configuration package from within the code.

#### `void stop_all_sensors()`

Turn of all sensors and stop streaming sensor data via ble.

### Sensor Configuration

Via the EdgeML pipeline or the `configure_sensor` function the sensor, PDM mic, and audio playback can be started or stopped.

The sensor IDs can be found in the "src/custom_sensor/SensorID_Earable.h".

A configuration packet is implemented as struct:

```c++
struct SensorConfigurationPacket {
    uint8_t sensorId{};
    float sampleRate{};
    uint32_t latency{};
};
```

| Byte 0    | Byte 1-4     | Byte 5-8 |
|-----------|--------------|----------|
| SensorID  | Sample Rate  | Latency  |
| uint8     | float        | uint32   |

SensorID: ID of the sensor.<br>
Sample Rate: Desired sample rate.<br>
Latency: Legacy field which is mostly ignored. However, it has been repurposed as shown later.

Each sensor or audio IO can be enabled individually or together at the same time with predefined configurations.
It is recommended to use the predefined configurations.

The available predefined Sensors:

#### IMU
Sensor ID: 0

The IMU provides acceleration, gyroscope, and magnetometer values in xyz.
<br>
(max. 50Hz alone; max. 30Hz with other sensors)

Data Array structure:

| Byte 0-3 | Byte 4-7 | Byte 8-11 | Byte 12-15 | Byte 16-19 | Byte 20-23 | Byte 24-27 | Byte 28-31 | Byte 32-35 |
|----------|----------|-----------|------------|------------|------------|------------|------------|------------|
| IMU X    | IMU Y    | IMU Z     | GYRO X     | GYRO Y     | GYRO Z     | MAG X      | MAG Y      | MAG Z      |
| float    | float    | float     | float      | float      | float      | float      | float      | float      | 

#### BME280
Sensor ID: 1

The BME provides in ear air pressure measurements as well as temperature data of the earable.
<br>
(max. 50Hz alone; max. 30Hz with other sensors)

Data Array structure:

| Byte 0-3 | Byte 4-7    |
|----------|-------------|
| PRESSURE | TEMPERATURE |
| float    | float       |

#### PDM MIC
Sensor ID: 2

The PDM Microphone provides audio data up to 62.5kHz.
The sample rate files of the configuration package is the audio sample rate of the sensor.

The available sampling rates are:

- 16000Hz
- 20000Hz
- 25000Hz
- 31250Hz
- 33333Hz
- 40000Hz
- 41667Hz
- 50000Hz
- 62500Hz

Most recommended are:
- 16000Hz
- 41667Hz
- 62500Hz

### Button

The earable features a button at its side. A software debounced interface is already included with the `earable_btn` Button instance.

It includes the following functionality:

#### `ButtonState getState()`

Returns the button state either being
- 0: RELEASED
- 1: PRESSED

The button state is updated by interrupt.

#### `void setDebounceTime(unsigned long debounceTime)`

Set debounce time in ms. (Default 25ms)

## LED

The earable features an RGB LED.

It includes the following functionality:

#### `void set_color(Color col);

Set the RGB color of the LED.

```c++
RGBColor red = {255,0,0};
earable_led.set_color(red);
```

## Cite
```bib
@inproceedings{10.1145/3544793.3563415,
	title        = {OpenEarable: Open Hardware Earable Sensing Platform},
	author       = {Röddiger, Tobias and King, Tobias and Roodt, Dylan Ray and Clarke, Christopher and Beigl, Michael},
	year         = 2023,
	booktitle    = {Proceedings of the 1st International Workshop on Earable Computing},
	location     = {Cambridge, United Kingdom},
	publisher    = {Association for Computing Machinery},
	address      = {New York, NY, USA},
	series       = {EarComp’22},
	pages        = {29–34},
	doi          = {10.1145/3544793.3563415},
	url          = {https://doi.org/10.1145/3544793.3563415},
	numpages     = 6,
	keywords     = {In-Ear Headphones, IMU, Monitoring}
}
```

[earable]: ./img/earable.png
