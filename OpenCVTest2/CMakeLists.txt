add_executable(opencvtest2  OpenCVTest2.cpp stdafx2.cpp)

find_package(OpenCV CONFIG REQUIRED)
# imread/imshow 需要 imgcodecs/highgui, 旧清单缺这两个库导致 Windows 链接失败
target_link_libraries(opencvtest2 PRIVATE
    opencv_core opencv_ml opencv_dnn opencv_flann
    opencv_imgcodecs opencv_highgui)
