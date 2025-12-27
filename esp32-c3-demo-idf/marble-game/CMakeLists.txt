# ESP32-C3 Marble Game - ESP-IDF Project
cmake_minimum_required(VERSION 3.16)

# Set the components to include
set(EXTRA_COMPONENT_DIRS "../components")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(marble-game)
