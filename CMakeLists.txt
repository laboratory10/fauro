####
# This sets up the build system for the 'fauro_project' project, including
# components and deployments from project.cmake. In addition, it imports the core F Prime components.
####

cmake_minimum_required(VERSION 3.13)
project(fauro_project C CXX)

### Libraries
# Set the path to the Arduino libraries directory
set(ARDUINO_LIB_PATH "/home/snow/Arduino/libraries")
set(ARDUINO_SAMD_LIB_PATH "/home/snow/.arduino15/packages/adafruit/hardware/samd/1.7.16/libraries")

# Include directories for Arduino libraries
include_directories(
    ${ARDUINO_LIB_PATH}/Time
    ${ARDUINO_LIB_PATH}/RadioHead
    ${ARDUINO_LIB_PATH}/Adafruit_Unified_Sensor
    ${ARDUINO_LIB_PATH}/Adafruit_BMP3XX_Library
    ${ARDUINO_LIB_PATH}/Adafruit_GPS_Library
    ${ARDUINO_LIB_PATH}/Adafruit_LS3MDL
    ${ARDUINO_LIB_PATH}/Adafruit_LSM6DS
    ${ARDUINO_SAMD_LIB_PATH}/Wire
    ${ARDUINO_SAMD_LIB_PATH}/SPI
    ${ARDUINO_SAMD_LIB_PATH}/Adafruit_ZeroDMA
)

###
# F' Core Setup
# This includes all of the F prime core components, and imports the make-system.
###
include("${CMAKE_CURRENT_LIST_DIR}/fprime/cmake/FPrime.cmake")
# NOTE: register custom targets between these two lines
include("${FPRIME_FRAMEWORK_PATH}/cmake/FPrime-Code.cmake")


# This includes project-wide objects
include("${CMAKE_CURRENT_LIST_DIR}/project.cmake")
