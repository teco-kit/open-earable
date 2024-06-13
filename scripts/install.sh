#!/bin/bash
# Install board
echo "Installing mbed_nano board version"
arduino-cli core install arduino:mbed_nano

# Install dependecies
# Install EDGEML (1.3.3)
echo "Installing libraries..."
arduino-cli lib install EdgeML-Arduino@1.3.3
arduino-cli lib install 'Adafruit BMP280 Library'
arduino-cli lib install "DFRobot_BMX160"
arduino-cli lib install "SdFat - Adafruit Fork"

tmp_output=$(arduino-cli core search arduino:mbed_nano | grep arduino:mbed_nano)
MBED_NANO_VERSION=$(echo "$tmp_output" | grep -Eo '[0-9]+\.[0-9]+\.[0-9]+')
$version_number

echo $MBED_NANO_VERSION

OS=$(uname)

# Path to your Arduino15 folder (replace with your actual path)
if [[ "$OS" == "Linux" ]]; then
  echo "This is a Linux system."
  ARDUINO_15_PATH="/home/$(whoami)/.arduino15"
elif [[ "$OS" == "Darwin" ]]; then
  echo This is a macOS system.
  ARDUINO_15_PATH="/Users/$(whoami)/Library/Arduino15"
fi

echo $ARDUINO_15_PATH

# Resources folder path (replace with your actual path)
RESOURCES_PATH="../resources"

# Check if Arduino15 folder exists
if [ ! -d "$ARDUINO_15_PATH" ]; then
  echo "Error: Arduino15 folder not found at $ARDUINO_15_PATH"
  exit 1
fi

# Check if Resources folder exists
if [ ! -d "$RESOURCES_PATH" ]; then
  echo "Error: Resources folder not found at $RESOURCES_PATH"
  exit 1
fi

# SPI, Wire, and Variant Setup
SPI_DIR="$RESOURCES_PATH/spi_files/SPI"
WIRE_DIR="$RESOURCES_PATH/wire_files/Wire"
VARIANT_DIR="$RESOURCES_PATH/variant"

# Create new board for OpenEarable
BOARD_DIR="$ARDUINO_15_PATH/packages/arduino/hardware/mbed_nano/$MBED_NANO_VERSION"
NEW_BOARD_DIR="$ARDUINO_15_PATH/packages/arduino/hardware/openearable/$MBED_NANO_VERSION"

echo "Creating folder for new Openearable board..."
mkdir "$ARDUINO_15_PATH/packages/arduino/hardware/openearable"
cp -R $BOARD_DIR $NEW_BOARD_DIR
rm -rf "$NEW_BOARD_DIR/variants/NANO_RP20240_CONNECT"

echo "Replacing board names..."
if [[ "$OS" == "Linux" ]]; then
  sed -i '/^nanorp2040connect/d' "$NEW_BOARD_DIR/boards.txt"
  sed -i 's/nano33ble.name=Arduino Nano 33 BLE/nano33ble.name=Openearable/' "$NEW_BOARD_DIR/boards.txt"
elif [[ "$OS" == "Darwin" ]]; then
  sed -i="" '/^nanorp2040connect/d' "$NEW_BOARD_DIR/boards.txt"
  sed -i="" 's/nano33ble.name=Arduino Nano 33 BLE/nano33ble.name=Openearable/' "$NEW_BOARD_DIR/boards.txt"
fi

LIB_DIR="$NEW_BOARD_DIR/libraries/"

# Replace SPI library
echo "Replacing SPI library..."
rm -rf "$LIB_DIR/SPI"
cp -r "$SPI_DIR" "$LIB_DIR"

# Replace Wire library
echo "Replacing Wire library..."
rm -rf "$LIB_DIR/Wire"
cp -r "$WIRE_DIR" "$LIB_DIR"

# Copy RingBuffer files
echo "Copying RingBuffer files..."
cp "$RESOURCES_PATH/wire_files/RingBuffer.h" "$ARDUINO_15_PATH/packages/arduino/hardware/openearable/$MBED_NANO_VERSION/cores/arduino"
cp "$RESOURCES_PATH/wire_files/RingBuffer.cpp" "$ARDUINO_15_PATH/packages/arduino/hardware/openearable/$MBED_NANO_VERSION/cores/arduino"

# Replace nrfx_spim files
echo "Replacing nrfx_spim files..."
cp "$RESOURCES_PATH/spi_files/nrfx_spim.h" "$ARDUINO_15_PATH/packages/arduino/hardware/openearable/$MBED_NANO_VERSION/cores/arduino/mbed/targets/TARGET_NORDIC/TARGET_NRF5x/TARGET_SDK_15_0/modules/nrfx/drivers/include/"
cp "$RESOURCES_PATH/spi_files/nrfx_spim.c" "$ARDUINO_15_PATH/packages/arduino/hardware/openearable/$MBED_NANO_VERSION/cores/arduino/mbed/targets/TARGET_NORDIC/TARGET_NRF5x/TARGET_SDK_15_0/modules/nrfx/drivers/src/"

# Replace variant files
echo "Replacing variant files..."
cp "$RESOURCES_PATH/variant/pins_arduino.h" "$ARDUINO_15_PATH/packages/arduino/hardware/openearable/$MBED_NANO_VERSION/variants/ARDUINO_NANO33BLE"
cp "$RESOURCES_PATH/variant/variant.cpp" "$ARDUINO_15_PATH/packages/arduino/hardware/openearable/$MBED_NANO_VERSION/variants/ARDUINO_NANO33BLE"

# sdFat Library Setup
SDFAT_LIB_DIR="$HOME/Documents/Arduino/libraries/SdFat_-_Adafruit_Fork"  # Update path if needed

# Check if sDFat library exists
if [ ! -d "$SDFAT_LIB_DIR" ]; then
  echo "Warning: SdFat library not found at $SDFAT_LIB_DIR"
echo "Please install the SdFat library from Bill Greiman."
exit 1
fi
# Replace SdFatConfig.h
echo "Replacing SdFatConfig.h..."
cp "$RESOURCES_PATH/sdfat_config/SdFatConfig.h" "$SDFAT_LIB_DIR/"

# sdFat Library Setup
BMP280_LIB_DIR="$HOME/Documents/Arduino/libraries/Adafruit_BMP280_Library"  # Update path if needed

# Check if sDFat library exists
if [ ! -d "$BMP280_LIB_DIR" ]; then
  echo "Warning: BMP280 library not found at $BMP280_LIB_DIR"
echo "Please install the BMP280 library."
exit 1
fi
# Replace SdFatConfig.h
echo "Replacing BMP280 Files..."
cp "$RESOURCES_PATH/Adafruit_BMP280_Library/Adafruit_BMP280.cpp" "$BMP280_LIB_DIR/"
cp "$RESOURCES_PATH/Adafruit_BMP280_Library/Adafruit_BMP280.h" "$BMP280_LIB_DIR/"

echo "Done. If you have issues, please reboot first."