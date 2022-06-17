cmake_minimum_required(VERSION 3.4)

project(Colt-Lang VERSION 0.0.15.4 LANGUAGES C)

file(GLOB_RECURSE ColtHeaders "colt/src/*.h")
file(GLOB_RECURSE ColtUnits "colt/src/*.c")

add_executable(colt
	${ColtHeaders} ${ColtUnits}
)

# Set default build type to Release
set(CMAKE_BUILD_TYPE "Release")

# Interpreter ABI
set(COLTI_ABI_MAJOR 0)
set(COLTI_ABI_MINOR 0)
set(COLTI_ABI_PATCH 0)
set(COLTI_ABI_TWEAK 4)

target_include_directories(colt PUBLIC "${CMAKE_SOURCE_DIR}/colt/src/util" "${CMAKE_SOURCE_DIR}/colt/src/byte-code" "${CMAKE_SOURCE_DIR}/colt/src")

set(VS_STARTUP_PROJECT colt)

target_precompile_headers(colt PUBLIC 
	"$<$<COMPILE_LANGUAGE:C>:${PROJECT_SOURCE_DIR}/colt/src/util/precomph.h>")

# Define COLT_DEBUG_BUILD for debug config
target_compile_definitions(colt PRIVATE $<$<CONFIG:DEBUG>:COLT_DEBUG_BUILD>)

# Useful macros for knowing which compiler is going to compile the code
set(IMPL_COLT_CLANG 0)
set(IMPL_COLT_GNU 0)
set(IMPL_COLT_INTEL 0)
set(IMPL_COLT_MSVC 0)

if (CMAKE_C_COMPILER_ID MATCHES "Clang") # AppleClang or Clang
	set(IMPL_COLT_CLANG 1)
elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU")
	set(IMPL_COLT_GNU 1)
elseif (CMAKE_C_COMPILER_ID STREQUAL "Intel")
	set(IMPL_COLT_INTEL 1)
elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
	set(IMPL_COLT_MSVC 1)
endif()

set(IMPL_COLT_OS_WINDOWS 0)
set(IMPL_COLT_OS_LINUX 0)
set(IMPL_COLT_OS_APPLE 0)

if(WIN32)
	set(IMPL_COLT_OS_WINDOWS 1)
	set(IMPL_COLT_OS_STRING "Windows")
elseif(APPLE)
	set(IMPL_COLT_OS_APPLE 1)
	set(IMPL_COLT_OS_STRING "Apple")
elseif(UNIX AND NOT APPLE) #LINUX
	set(IMPL_COLT_OS_LINUX 1)
	set(IMPL_COLT_OS_STRING "Linux")
else()
	set(IMPL_COLT_OS_STRING "UNKNOWN")
endif()

configure_file("${CMAKE_SOURCE_DIR}/resources/cmake/cmake_colt_config.in"
	"${CMAKE_SOURCE_DIR}/colt/src/util/colt_config.h")


# Testing for CTest
enable_testing()
# Run the executable
add_test(NAME RunColt
	COMMAND colt)

# DOXYGEN
option(BUILD_DOC "Build documentation" ON)

# check if Doxygen is installed
find_package(Doxygen)
message(STATUS "Searching for Doxygen executable...")
if (DOXYGEN_FOUND)
	message(STATUS "Found Doxygen executable.")
	# set input and output files
	set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/resources/doxygen/Doxyfile.in)
	set(DOXYGEN_OUT ${CMAKE_SOURCE_DIR}/docs/Doxyfile.out)

	message(STATUS "Configuring Doxygen input file...")
	# request to configure the file
	configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT})

	add_custom_target(GENERATE_DOCS
		COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		COMMENT "Generating API documentation with Doxygen"
		VERBATIM)
else (DOXYGEN_FOUND)
	message(WARNING "Doxygen was not found! Doxygen need to be installed to generate the documentation!")
endif (DOXYGEN_FOUND)