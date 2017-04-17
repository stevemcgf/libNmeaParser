cmake_minimum_required(VERSION 2.8)

# Get the latest abbreviated commit hash of the working branch
execute_process(
	COMMAND git log -1 --format=%h
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	OUTPUT_VARIABLE GIT_COMMIT_HASH
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the latest tag version
execute_process(
	COMMAND git describe --tags
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	OUTPUT_VARIABLE GIT_TAG_VERSION
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

if (NOT "${GIT_TAG_VERSION}" STREQUAL "")
	string(REGEX REPLACE "^v([0-9]+)\\..*" "\\1" VERSION_MAJOR "${GIT_TAG_VERSION}")
	string(REGEX REPLACE "^v[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${GIT_TAG_VERSION}")
	string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${GIT_TAG_VERSION}")
	set(VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
	set(VERSION_FULL "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}-${GIT_COMMIT_HASH}")
endif (NOT "${GIT_TAG_VERSION}" STREQUAL "")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Version.h.in ${CMAKE_CURRENT_BINARY_DIR}/Version.h @ONLY)

file(GLOB lib_SRC "include/*.h" "src/*.cpp")

add_compile_options(-std=c++11)
add_compile_options(-DBOOST_LOG_DYN_LINK)
add_compile_options(-include ${CMAKE_CURRENT_BINARY_DIR}/Version.h)

find_package(Boost 1.54 REQUIRED COMPONENTS log regex thread)

include_directories(${Boost_INCLUDE_DIRS})

add_library(NmeaParser ${lib_SRC})
target_include_directories(NmeaParser PUBLIC "include")

target_link_libraries(NmeaParser ${Boost_LIBRARIES})

if (NOT "${VERSION_STRING}" STREQUAL "")

	set_target_properties(NmeaParser PROPERTIES VERSION ${VERSION_STRING} SOVERSION ${VERSION_MAJOR})
	
endif (NOT "${VERSION_STRING}" STREQUAL "")

# Unit Testing

add_executable(test.libNmeaParser test/test.cpp)
target_link_libraries (test.libNmeaParser NmeaParser)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DNP_DEBUG")

enable_testing ()
add_test (NAME NmeaParserTest COMMAND test.libNmeaParser)

# add a target to generate API documentation with Doxygen

find_package(Doxygen)

if(DOXYGEN_FOUND)

	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
	add_custom_target(docs.NmeaParser
		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen" VERBATIM
		)

endif(DOXYGEN_FOUND)
