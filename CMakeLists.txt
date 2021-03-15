# CMakeLists.txt
cmake_minimum_required(VERSION 3.15)
project(project_4)

# Use C++ 17 features
set(CMAKE_CXX_STANDARD 17)

message("CMAKE_OSX_ARCHITECTURES: ${CMAKE_OSX_ARCHITECTURES}")
message("CMAKE_HOST_SYSTEM_PROCESSOR: ${CMAKE_HOST_SYSTEM_PROCESSOR}")
message("CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
message("CMAKE_CROSSCOMPILING: ${CMAKE_CROSSCOMPILING}")

# Get includes directory
include_directories("./include/")

# Get header files
file(GLOB Hpp "./include/*.hpp")

# Get source files
file(GLOB Cpp "./src/*.cpp")

# Specify executable target
add_executable(project_4 ${Hpp} ${Cpp})

# Find OpenCV package
find_package(OpenCV REQUIRED)

message(STATUS "OpenCV library status:")
message(STATUS "    opencv: ${OpenCV}")
message(STATUS "    config: ${OpenCV_DIR}")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

# Link executable to OpenCV libraries
target_link_libraries(project_4 PRIVATE ${OpenCV_LIBS} )
