cmake_minimum_required(VERSION 2.8)

file(GLOB lib_SRC "src/*.h" "src/*.cpp")

add_library(NmeaParser ${lib_SRC})
