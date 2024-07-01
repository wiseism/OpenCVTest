# 判断操作系统
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")  
    # macOS 的情况  

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")  
    # Windows 的情况（你可能需要设置不同的路径或使用其他方式找到头文件）  
else()
    # Linux 或其他 Unix-like 系统的情况  
endif()

add_executable(opencvtest2  OpenCVTest2.cpp stdafx2.cpp)