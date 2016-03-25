cmake_minimum_required(VERSION 2.8)

file(GLOB lib_SRC "src/*.h" "src/*.cpp")

add_library(NmeaParser ${lib_SRC})
target_include_directories(NmeaParser PUBLIC "src")

target_link_libraries(NmeaParser ${Boost_LIBRARIES})

add_executable(test.NmeaParser test/test.cpp)
target_link_libraries (test.NmeaParser NmeaParser)

add_test (NAME libNmeaParser COMMAND test.NmeaParser)
