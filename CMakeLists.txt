include_directories("/usr/local/include")
link_directories(/usr/local/lib)
link_libraries(opencv_shape opencv_stitching opencv_objdetect opencv_superres opencv_videostab opencv_calib3d opencv_features2d opencv_highgui opencv_videoio opencv_imgcodecs opencv_video opencv_photo opencv_ml opencv_imgproc opencv_flann opencv_core)
cmake_minimum_required(VERSION 3.6)
project(video)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -I /usr/local/include/ff -lpthread -pthread")

set(SOURCE_FILES
		src/block.h
		src/graph.h
		src/graph.cpp
		src/maxflow.cpp
		src/carving.hpp
		src/carving.cpp
		src/video.hpp
		src/video.cpp
		src/main.cpp
		src/min.cpp
		src/pmin.cpp
    )

add_executable(video ${SOURCE_FILES})
add_executable(fancy ${SOURCE_FILES})