
add_executable(opencvtest2  OpenCVTest2.cpp stdafx2.cpp)
# 判断操作系统
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")  
    # macOS 的情况  
    # this is heuristically generated, and may not be correct
    find_package(OpenCV CONFIG REQUIRED)
    # note: 11 additional targets are not displayed.
    target_link_libraries(opencvtest2 PRIVATE opencv_ml opencv_dnn opencv_core opencv_flann)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")  
    # Windows 的情况（你可能需要设置不同的路径或使用其他方式找到头文件）  
else()
    # Linux 或其他 Unix-like 系统的情况  
endif()
