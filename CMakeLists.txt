cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(compage VERSION 0.3)

# Specify the C++ standard
# set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_STANDARD_REQUIRED True)

# Export compile commands for the LSP mode
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Defines / Options
OPTION(DEBUG "Turns on full debugging of both libraries" OFF)

IF(DEBUG)
  ADD_DEFINITIONS(-DDEBUG)
ENDIF(DEBUG)


# compage library an usage examples
add_subdirectory(compage)
add_subdirectory(examples)
