cmake_minimum_required(VERSION 2.8)

file(GLOB lib_SRC "include/*.h" "src/*.cpp")

add_compile_options(-std=c++11)
add_compile_options(-DBOOST_LOG_DYN_LINK)

find_package(Boost 1.55 REQUIRED COMPONENTS log regex thread)

include_directories(${Boost_INCLUDE_DIRS})

add_library(NmeaParser ${lib_SRC})
target_include_directories(NmeaParser PUBLIC "include")

target_link_libraries(NmeaParser ${Boost_LIBRARIES})

add_executable(test.libNmeaParser test/test.cpp)
target_link_libraries (test.libNmeaParser NmeaParser)
