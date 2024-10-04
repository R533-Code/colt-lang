cmake_minimum_required(VERSION 3.21)

include(FetchContent)

# C++20 is required
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(EmptyPlugin-Plugin VERSION 0.0.0.0 LANGUAGES CXX)
set(BUILD_SHARED_LIBS FALSE CACHE BOOL "" FORCE)

# Save all the files to compile
file(GLOB_RECURSE PluginHeaders "src/*.h")
file(GLOB_RECURSE PluginUnits "src/*.cpp")

# Create the shared library
add_library(EmptyPlugin SHARED
  ${PluginUnits} ${PluginHeaders}
)

#########################################
# COLT-CPP
message(STATUS "Adding 'colt-cpp'...")
FetchContent_Declare(
  "coltcpp"
  GIT_REPOSITORY "https://github.com/R533-Code/colt-cpp"
  GIT_TAG main
)
FetchContent_MakeAvailable(coltcpp)

target_link_libraries(EmptyPlugin PUBLIC colt::coltcpp)
message(STATUS "Added 'colt-cpp'!
")
#########################################
