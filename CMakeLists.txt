cmake_minimum_required(VERSION 3.10.0)
project(steamScrapers VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core WebEngineWidgets Widgets Sql)

add_executable(steamScrapers 
main.cpp 
include/includes.h
include/steamReader.h 
src/steamReader.cpp 
include/parser.h 
src/parser.cpp)

target_link_libraries(steamScrapers PRIVATE Qt6::Core Qt6::Widgets Qt6::WebEngineWidgets Qt6::Sql)