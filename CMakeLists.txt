cmake_minimum_required(VERSION 3.21)

# C++20 is required
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(Colt-Compiler VERSION 0.0.0.0 LANGUAGES CXX)
set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)

# Save all the files to compile
file(GLOB_RECURSE ColtHeaders "src/*.h")
file(GLOB_RECURSE ColtUnits "src/*.cpp")
file(GLOB_RECURSE ColtGithubActions ".github/workflows/*.yml")
file(GLOB ColtDesignFiles "resources/*.md")
file(GLOB_RECURSE ColtTestUnits "test/*.cpp")
# Save test files
file(GLOB_RECURSE ColtASTTestsPath "resources/tests/ast/*.ct")
file(GLOB_RECURSE ColtLexerTestsPath "resources/tests/lexer/*.ct")

# Name of the compiler executable
set(COLT_EXECUTABLE_NAME coltc)

set(CMAKE_ENABLE_EXPORTS True)
# Create the compiler executable
add_executable(${COLT_EXECUTABLE_NAME}
  ${ColtHeaders} ${ColtUnits}
  ${ColtASTTestsPath} ${ColtLexerTestsPath}
  ${ColtGithubActions} ${ColtDesignFiles}
)
if (WIN32)
  target_sources(${COLT_EXECUTABLE_NAME} PRIVATE "resources/icon/colt.rc")
endif()

# Create compiler tests, with its custom main.
list(REMOVE_ITEM ColtUnits "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
add_executable(coltc_test
  ${ColtHeaders} ${ColtUnits} ${ColtTestUnits}
)

# Add precompiled header
target_precompile_headers(${COLT_EXECUTABLE_NAME} PUBLIC 
  "$<$<COMPILE_LANGUAGE:CXX>:${PROJECT_SOURCE_DIR}/src/colt_pch.h>")
target_precompile_headers(coltc_test PUBLIC 
  "$<$<COMPILE_LANGUAGE:CXX>:${PROJECT_SOURCE_DIR}/src/colt_pch.h>")

# Define COLT_DEBUG_BUILD for debug config
target_compile_definitions(${COLT_EXECUTABLE_NAME} PRIVATE
  $<$<CONFIG:Debug>:COLT_DEBUG;COLT_DEBUG_BUILD> _CRT_SECURE_NO_WARNINGS
)
target_compile_definitions(coltc_test PRIVATE
  $<$<CONFIG:Debug>:COLT_DEBUG;COLT_DEBUG_BUILD> _CRT_SECURE_NO_WARNINGS
)

# The colt compiler is the startup project in Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${COLT_EXECUTABLE_NAME})

target_compile_options(
  ${COLT_EXECUTABLE_NAME} PUBLIC 
  $<$<BOOL:${MSVC}>:/Zc:preprocessor;/external:W0;/external:anglebrackets>
  $<$<NOT:$<BOOL:${MSVC}>>:-fvisibility=hidden>
)
target_compile_options(
  coltc_test PUBLIC  
  $<$<BOOL:${MSVC}>:/Zc:preprocessor;/external:W0;/external:anglebrackets>
  $<$<NOT:$<BOOL:${MSVC}>>:-fvisibility=hidden>
)

#########################################
# LIBRARIES SETUP
#########################################

message(STATUS "Setting up libraries...\n")

#########################################
# LIBFFI
message(STATUS "Adding 'libffi'...")
add_subdirectory("${PROJECT_SOURCE_DIR}/libraries/libffi")
target_link_libraries(${COLT_EXECUTABLE_NAME} PUBLIC ffi::ffi)
target_link_libraries(coltc_test PUBLIC ffi::ffi)
message(STATUS "Added 'libffi'!\n")
#########################################

#########################################
# COLT-CPP
message(STATUS "Adding 'colt-cpp'...")
add_subdirectory("${PROJECT_SOURCE_DIR}/libraries/colt-cpp")
target_link_libraries(${COLT_EXECUTABLE_NAME} PUBLIC colt::coltcpp)
target_link_libraries(coltc_test PUBLIC colt::coltcpp)
message(STATUS "Added 'colt-cpp'!\n")
#########################################

# Directories for '#include <...>'
target_include_directories(${COLT_EXECUTABLE_NAME} PUBLIC 
  "${CMAKE_SOURCE_DIR}/src"
  "${CMAKE_SOURCE_DIR}/src/frontend"
  "${CMAKE_SOURCE_DIR}/src/backend"
  SYSTEM # So no warning is shown
  "${CMAKE_SOURCE_DIR}/libraries/colt-cpp/include"
  #"${CMAKE_SOURCE_DIR}/libraries/llvm-project/llvm/include"
  #"${CMAKE_BINARY_DIR}/libraries/llvm-project/llvm/include"
)
target_include_directories(coltc_test PUBLIC 
  "${CMAKE_SOURCE_DIR}/src"
  "${CMAKE_SOURCE_DIR}/src/frontend"
  "${CMAKE_SOURCE_DIR}/src/backend"
  SYSTEM # So no warning is shown
  "${CMAKE_SOURCE_DIR}/libraries/colt-cpp/include"
  "${PROJECT_SOURCE_DIR}/test"
  #"${CMAKE_SOURCE_DIR}/libraries/llvm-project/llvm/include"
  #"${CMAKE_BINARY_DIR}/libraries/llvm-project/llvm/include"
)

###########################
# Catch2
message(STATUS "Adding 'Catch2'...")
target_include_directories(coltc_test PUBLIC "${PROJECT_SOURCE_DIR}/libraries/colt-cpp/libraries/Catch2")
target_link_libraries(coltc_test PUBLIC Catch2::Catch2)
catch_discover_tests(coltc_test DISCOVERY_MODE PRE_TEST)
add_dependencies(coltc_test coltcpp_tests)
message(STATUS "Added 'Catch2'!\n")
###########################

# Copy all needed dlls to executable directory
copy_all_dl_to_bin(${COLT_EXECUTABLE_NAME})
copy_all_dl_to_bin(coltc_test)

#########################################
# COLT TESTS
#########################################

#message(STATUS "Searching for tests...")
# Load tests
list(LENGTH ColtASTTestsPath testCountAST)
list(LENGTH ColtLexerTestsPath testCountLexer)
#message(STATUS "Found " ${testCountAST} " AST tests and " ${testCountLexer} " Lexer tests.")

# For VS, group tests
source_group("Tests AST" FILES ${ColtASTTestsPath})
source_group("Tests Lexer" FILES ${ColtLexerTestsPath})
source_group("Github Actions" FILES ${ColtGithubActions})
source_group("Colt Design" FILES ${ColtDesignFiles})

#########################################
# DOXYGEN
option(BUILD_DOC "Build documentation" ON)

message(STATUS "Searching for Doxygen executable...")
# check if Doxygen is installed
find_package(Doxygen)

if (DOXYGEN_FOUND)
  message(STATUS "Found Doxygen executable.")
  # set input and output files
  set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/resources/doxygen/Doxyfile.in)
  set(DOXYGEN_OUT ${CMAKE_SOURCE_DIR}/docs/Doxyfile.out)

  message(STATUS "Configuring Doxygen input file...\n")
  # request to configure the file
  configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT})

  add_custom_target(GENERATE_DOCS
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM
  )
else (DOXYGEN_FOUND)
  message(WARNING "Doxygen was not found! Doxygen need to be installed to generate the documentation!\n")
endif (DOXYGEN_FOUND)
#########################################

#########################################
# COLT CONFIG
message(STATUS "Configuring 'colt_config.h'...")
configure_file("${CMAKE_SOURCE_DIR}/resources/cmake/cmake_colt_config.in" "${CMAKE_SOURCE_DIR}/src/colt_config.h")
message(STATUS "Finished configuring 'colt_config.h'!\n")
#########################################

#########################################
# Configure CPack
# set(CPACK_PROJECT_NAME "${PROJECT_NAME}")
# set(CPACK_PROJECT_VERSION "${PROJECT_VERSION}")
# 
# # Enable NSIS generator
# set(CPACK_GENERATOR "NSIS")
# 
# # Set NSIS-related options
# set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/icon/Colt-Logo-Installer.ico")
# set(CPACK_NSIS_MUI_UNICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/icon/Colt-Logo-Installer.ico")
# set(CPACK_NSIS_DISPLAY_NAME "${PROJECT_NAME} ${PROJECT_VERSION}")
# 
# # Include custom NSIS scripts
# list(APPEND CPACK_NSI_INCLUDES
#   "${CMAKE_CURRENT_SOURCE_DIR}/resources/colt-compiler.nsh"
# )
# 
# # Generate the installer
# include(CPack)
#########################################