cmake_minimum_required(VERSION 3.10.0)
project(steamScrapers VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

execute_process(COMMAND pg_config --includedir OUTPUT_VARIABLE PG_INCLUDE_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND pg_config --libdir OUTPUT_VARIABLE PG_LIB_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)

include_directories(${PG_INCLUDE_DIR})
link_directories(${PG_LIB_DIR})

find_package(Qt6 REQUIRED COMPONENTS Core WebEngineWidgets Widgets)

add_executable(steamScrapers 
main.cpp 
include/includes.h
include/steamReader.h 
src/steamReader.cpp 
include/parser.h 
src/parser.cpp
include/controller.h
src/controller.cpp
include/itemReader.h
src/itemReader.cpp)

target_link_libraries(steamScrapers PRIVATE Qt6::Core Qt6::Widgets Qt6::WebEngineWidgets pq)