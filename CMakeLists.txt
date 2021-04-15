# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

cmake_minimum_required(VERSION 3.5)
project(DuckHax)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include_directories(
    include
    src
)

set (SRCS
    src/autodumper.cpp
    src/bitmapparser.cpp
    src/dpcparser.cpp
    src/main.cpp
    src/meshparser.cpp
    src/parser.cpp
    src/parsinghelper.cpp
    src/zounaclasses.cpp
    src/soundparser.cpp
    src/sdxparser.cpp
)


add_executable(${CMAKE_PROJECT_NAME} ${SRCS})
target_link_libraries(DuckHax -lpng -lz)

