cmake_minimum_required(VERSION 2.8.8)
project(atu)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/firmware/)

set(RESET_ANIMATION OFF)
set(POWER_DOWN_ON_IDLE OFF)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(RESET_ANIMATION ON)
  set(POWER_DOWN_ON_IDLE OFF)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

SET(FIRMWARE_VERSION_MAJOR 0)
SET(FIRMWARE_VERSION_MINOR 1)
configure_file(${PROJECT_SOURCE_DIR}/firmware/config.h.in ${PROJECT_BINARY_DIR}/firmware/config.h)

# firmware sources
add_subdirectory(firmware)

