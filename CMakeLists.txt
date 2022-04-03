cmake_minimum_required(VERSION 2.8.8)
project(atu)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/firmware/)

set(RESET_ANIMATION OFF)
set(POWER_DOWN_ON_IDLE OFF)
set(LOG_TUING OFF)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(RESET_ANIMATION OFF)
  set(POWER_DOWN_ON_IDLE OFF)
  set(LOG_TUING ON)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

message(STATUS "Config: Reset animation: ${RESET_ANIMATION}")
message(STATUS "        Power down on idle: ${POWER_DOWN_ON_IDLE}")
message(STATUS "        Log to EEPROM: ${LOG_TUING}")

SET(FIRMWARE_VERSION_MAJOR 0)
SET(FIRMWARE_VERSION_MINOR 1)
SET(FIRMWARE_VERSION_PATCH 1)

configure_file(${PROJECT_SOURCE_DIR}/firmware/config.h.in
  ${PROJECT_BINARY_DIR}/firmware/config.h)

# firmware sources
add_subdirectory(firmware)

