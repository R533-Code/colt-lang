cmake_minimum_required(VERSION 3.4)

project(Colt-Interpreter VERSION 0.0.8.5 LANGUAGES C)

file(GLOB_RECURSE ColtiHeaders "colti/src/*.h")
file(GLOB_RECURSE ColtiUnits "colti/src/*.c")

add_executable(colti
	${ColtiHeaders} ${ColtiUnits}
)

target_include_directories(colti PUBLIC "${CMAKE_SOURCE_DIR}/colti/src/util" "${CMAKE_SOURCE_DIR}/colti/src/byte-code" "${CMAKE_SOURCE_DIR}/colti/src")

set(VS_STARTUP_PROJECT colti)

target_precompile_headers(colti PUBLIC 
	"$<$<COMPILE_LANGUAGE:C>:${PROJECT_SOURCE_DIR}/colti/src/util/precomph.h>")

# Define COLT_DEBUG_BUILD for debug config
target_compile_definitions(colti PRIVATE $<$<CONFIG:DEBUG>:COLTI_DEBUG_BUILD>)

# Useful macros for knowing which compiler is going to compile the code
set(IMPL_COLTI_CLANG 0)
set(IMPL_COLTI_GNU 0)
set(IMPL_COLTI_INTEL 0)
set(IMPL_COLTI_MSVC 0)

if (CMAKE_C_COMPILER_ID MATCHES "Clang") # AppleClang or Clang
	set(IMPL_COLTI_CLANG 1)
elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU")
	set(IMPL_COLTI_GNU 1)
elseif (CMAKE_C_COMPILER_ID STREQUAL "Intel")
	set(IMPL_COLTI_INTEL 1)
elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
	set(IMPL_COLTI_MSVC 1)
endif()

set(IMPL_COLTI_OS_WINDOWS 0)
set(IMPL_COLTI_OS_LINUX 0)
set(IMPL_COLTI_OS_APPLE 0)

if(WIN32)
	set(IMPL_COLTI_OS_WINDOWS 1)
	set(IMPL_COLTI_OS_STRING "Windows")
elseif(APPLE)
	set(IMPL_COLTI_OS_APPLE 1)
	set(IMPL_COLTI_OS_STRING "Apple")
elseif(UNIX AND NOT APPLE) #LINUX
	set(IMPL_COLTI_OS_LINUX 1)
	set(IMPL_COLTI_OS_STRING "Linux")
else()
	set(IMPL_COLTI_OS_STRING "UNKNOWN")
endif()

configure_file("${CMAKE_SOURCE_DIR}/resources/cmake/cmake_colti_config.in"
	"${CMAKE_SOURCE_DIR}/colti/src/util/colti_config.h")


# Testing for CTest
enable_testing()
# Run the executable
add_test(NAME RunColt
	COMMAND colti)

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