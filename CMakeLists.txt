cmake_minimum_required(VERSION 3.14)

# set the project name and version
project(cmake_watcher VERSION 1.0)

#Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CONAN_CMAKE_CXX_STANDARD ${CMAKE_CXX_STANDARD})
add_definitions("-std=c++17")

#Conan setup
execute_process(COMMAND "cd build && conan install .. && cd ..")
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

#Importing conan libraries
include_directories(${PROJECT_SOURCE_DIR}/src)

#Creating the main target
add_executable(cmake_watcher src/main.cpp src/FileWatcher.cpp src/ProgramHandler.cpp)

#Add libraries to the main target
target_link_libraries(cmake_watcher ${CONAN_LIBS})
include_directories(${PROJECT_BINARY_DIR})

#Inject build type information into config file
#Add optimization flag for release
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(cmake_watcher_PROD_MODE 0)
else()
	set(cmake_watcher_PROD_MODE 1)
	add_definitions("-Os" "-ffunction-sections" "-Wl,--gc-sections")
endif()

#Load the configuration file
configure_file(src/config.h.in "${CMAKE_BINARY_DIR}/config.h")