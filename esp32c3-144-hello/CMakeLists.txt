# Spotpear ESP32-C3 1.44" LCD - Hello World Demo
cmake_minimum_required(VERSION 3.16)

# Set the components to include
set(EXTRA_COMPONENT_DIRS "../esp32-c3-demo-idf/components")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp32c3-144-hello)
