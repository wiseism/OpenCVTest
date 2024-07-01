cmake_minimum_required(VERSION 3.12)
project(OpenCVTest)

set(CMAKE_CXX_STANDARD 11)
# 判断操作系统  
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")  
    # macOS 的情况  

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")  
    # Windows 的情况（你可能需要设置不同的路径或使用其他方式找到头文件）  
else()
    # Linux 或其他 Unix-like 系统的情况  
endif()

add_subdirectory(OpenCVTest2)