cmake_minimum_required(VERSION 3.16)
project(OpenCVTest CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# OpenCV 查找：同一份 CMakeLists 需同时覆盖两类环境——
#   1) 带 OpenCVConfig.cmake 的安装（Linux apt libopencv-dev、vcpkg 等）：
#      使用 find_package(OpenCV) 包配置模式；
#   2) Windows 官方 vc14/vc15 预编译包（如 3.2.0）：仅含 build\include 与
#      build\x64\vc14\lib，没有 CMake 包配置文件，回退为模块式查找
#      opencv_world320（Debug 为 opencv_world320d）。
# 预编译包根目录可用 -DOpenCVRoot=<dir> 或环境变量 OPENCV_DIR 覆盖，
# 默认 D:\opencv（与 .sln 构建的 windows-sln-build workflow 一致）。
# ---------------------------------------------------------------------------
find_package(OpenCV QUIET COMPONENTS core highgui imgcodecs)

if(NOT OpenCV_FOUND AND WIN32)
    if(DEFINED OpenCVRoot AND NOT OpenCVRoot STREQUAL "")
        set(_OpenCVRoot "${OpenCVRoot}")
    elseif(DEFINED ENV{OPENCV_DIR})
        set(_OpenCVRoot "$ENV{OPENCV_DIR}")
    else()
        set(_OpenCVRoot "D:/opencv")
    endif()

    find_path(OpenCV_INCLUDE_DIRS NAMES opencv2/core.hpp
              PATHS "${_OpenCVRoot}/build/include" NO_DEFAULT_PATH)
    find_library(OpenCV_LIB_RELEASE
                 NAMES opencv_world320 opencv_world
                 PATHS "${_OpenCVRoot}/build/x64/vc14/lib"
                       "${_OpenCVRoot}/build/x64/vc15/lib" NO_DEFAULT_PATH)
    find_library(OpenCV_LIB_DEBUG
                 NAMES opencv_world320d opencv_worldd
                 PATHS "${_OpenCVRoot}/build/x64/vc14/lib"
                       "${_OpenCVRoot}/build/x64/vc15/lib" NO_DEFAULT_PATH)

    set(OpenCV_LIBS "")
    if(OpenCV_LIB_DEBUG)
        list(APPEND OpenCV_LIBS debug "${OpenCV_LIB_DEBUG}")
    endif()
    if(OpenCV_LIB_RELEASE)
        list(APPEND OpenCV_LIBS optimized "${OpenCV_LIB_RELEASE}")
    endif()

    if(NOT OpenCV_INCLUDE_DIRS OR NOT OpenCV_LIBS)
        message(FATAL_ERROR
                "OpenCV not found under '${_OpenCVRoot}'. Put an official "
                "vc14/vc15 prebuilt package there, or pass -DOpenCVRoot=<dir> "
                "/ set OPENCV_DIR to its build parent.")
    endif()
    message(STATUS "OpenCV found (manual world lib): ${OpenCV_LIBS}")
elseif(NOT OpenCV_FOUND)
    message(FATAL_ERROR
            "OpenCV not found. On Ubuntu install it with: "
            "sudo apt-get install -y libopencv-dev")
endif()

add_executable(OpenCVTest OpenCVTest/OpenCVTest.cpp)
target_include_directories(OpenCVTest PRIVATE OpenCVTest)
if(OpenCV_INCLUDE_DIRS)
    target_include_directories(OpenCVTest PRIVATE ${OpenCV_INCLUDE_DIRS})
endif()
target_link_libraries(OpenCVTest PRIVATE ${OpenCV_LIBS})

enable_testing()

# ---------------------------------------------------------------------------
# OpenCV 示例程序（详细设计见 docs/opencv-examples.md）：
# examples/ 下每个 .cpp 编译为独立可执行文件，并注册到 CTest 便于回归。
# ---------------------------------------------------------------------------
file(GLOB _opencv_examples CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/examples/*.cpp")
foreach(_exe_src ${_opencv_examples})
    get_filename_component(_exe_name "${_exe_src}" NAME_WE)
    add_executable(${_exe_name} "${_exe_src}")
    if(OpenCV_INCLUDE_DIRS)
        target_include_directories(${_exe_name} PRIVATE ${OpenCV_INCLUDE_DIRS})
    endif()
    target_link_libraries(${_exe_name} PRIVATE ${OpenCV_LIBS})
    add_test(NAME ${_exe_name} COMMAND $<TARGET_FILE:${_exe_name}>)
endforeach()
