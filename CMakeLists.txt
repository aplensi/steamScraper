cmake_minimum_required(VERSION 3.10.0)
project(steamScrapers VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core WebEngineWidgets Widgets)

add_executable(steamScrapers main.cpp steamReader.h steamReader.cpp)

target_link_libraries(steamScrapers PRIVATE Qt6::Core Qt6::Widgets Qt6::WebEngineWidgets)