cmake_minimum_required(VERSION 3.16)
project(OpenCVTest CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenCV REQUIRED)

add_executable(OpenCVTest OpenCVTest/OpenCVTest.cpp)
target_include_directories(OpenCVTest PRIVATE OpenCVTest)
target_link_libraries(OpenCVTest PRIVATE ${OpenCV_LIBS})

enable_testing()
