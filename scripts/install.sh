#!/bin/bash

# Path to your Arduino15 folder (replace with your actual path)
ARDUINO_15_PATH="/path/to/Arduino15"

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

# Navigate to libraries directory
LIB_DIR="$ARDUINO_15_PATH/packages/arduino/hardware/mbed_nano/4.0.4/libraries"

# Check mbed_nano version (modify if needed)
MBED_NANO_VERSION="4.0.4"

# Install mbed_nano board (if not already installed)
if ! grep -q "mbed_nano:$MBED_NANO_VERSION" boards.txt; then
  echo "Installing mbed_nano board version $MBED_NANO_VERSION..."
  arduino-cli boards install arduino:mbed_nano:$MBED_NANO_VERSION
fi

# Replace SPI library
echo "Replacing SPI library..."
rm -rf "$LIB_DIR/SPI"
cp -r "$SPI_DIR" "$LIB_DIR"

# Replace Wire library
echo "Replacing Wire library..."
rm -rf "$LIB_DIR/Wire"
cp -r "$WIRE_DIR" "$LIB_DIR"
rm "$LIB_DIR/Wire.cpp"

# Copy RingBuffer files
echo "Copying RingBuffer files..."
cp "$RESOURCES_PATH/wire_files/RingBuffer.h" "$ARDUINO_15_PATH/packages/arduino/hardware/mbed_nano/$MBED_NANO_VERSION/cores/arduino"
cp "$RESOURCES_PATH/wire_files/RingBuffer.cpp" "$ARDUINO_15_PATH/packages/arduino/hardware/mbed_nano/$MBED_NANO_VERSION/cores/arduino"

# Replace nrfx_spim files
echo "Replacing nrfx_spim files..."
cp "$RESOURCES_PATH/spi_files/nrfx_spim.h" "$ARDUINO_15_PATH/packages/arduino/hardware/mbed_nano/$MBED_NANO_VERSION/cores/arduino/mbed/targets/TARGET_NORDIC/TARGET_NRF5x/TARGET_SDK_15_0/modules/nrfx/drivers/include"
cp "$RESOURCES_PATH/spi_files/nrfx_spim.c" "$ARDUINO_15_PATH/packages/arduino/hardware/mbed_nano/$MBED_NANO_VERSION/cores/arduino/mbed/targets/TARGET_NORDIC/TARGET_NRF5x/TARGET_SDK_15_0/modules/nrfx/drivers/src"

# Replace variant files
echo "Replacing variant files..."
cp "$RESOURCES_PATH/variant/pins_arduino.h" "$ARDUINO_15_PATH/packages/arduino/hardware/mbed_nano/$MBED_NANO_VERSION/variants/ARDUINO_NANO33BLE"
cp "$RESOURCES_PATH/variant/variant.cpp" "$ARDUINO_15_PATH/packages/arduino/hardware/mbed_nano/$MBED_NANO_VERSION/variants/ARDUINO_NANO33BLE"

# sdFat Library Setup
SDFAT_LIB_DIR="$HOME/Documents/Arduino/libraries/SdFat_-_Adafruit_Fork"  # Update path if needed

# Check if sDFat library exists
if [ ! -d "$SDFAT_LIB_DIR" ]; then
  echo "Warning: SdFat library not found at <span class="math-inline">SDFAT\_LIB\_DIR"
echo "Please install the SdFat library from Bill Greiman\."
exit 1
fi
\# Replace SdFatConfig\.h
echo "Replacing SdFatConfig\.h\.\.\."
cp "</span>