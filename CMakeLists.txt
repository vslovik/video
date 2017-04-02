include_directories("/usr/local/include")
link_directories(/usr/local/lib)
link_libraries(opencv_shape opencv_stitching opencv_objdetect opencv_superres opencv_videostab opencv_calib3d opencv_features2d opencv_highgui opencv_videoio opencv_imgcodecs opencv_video opencv_photo opencv_ml opencv_imgproc opencv_flann opencv_core)
cmake_minimum_required(VERSION 3.6)
project(video)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -I /usr/local/include/ff -lpthread -pthread")

set(SOURCE_FILES
		parallel/pcarving.cpp
		parallel/pcarving.h
		parallel/psobel.cpp
		parallel/psobel.h
		parallel/video.h
		parallel/video.cpp
		parallel/main.cpp
    )

set(SEQ_SOURCE_FILES
		sequential/carving.cpp
		sequential/carving.h
		sequential/svideo.h
		sequential/svideo.cpp
		sequential/main.cpp
		)

set(SEAM_SOURCE_FILES
		parallel/pcarving.cpp
		parallel/pcarving.h
		parallel/psobel.cpp
		parallel/psobel.h
		)

add_executable(video ${SOURCE_FILES})
add_executable(svideo ${SEQ_SOURCE_FILES})
add_executable(seam ${SEAM_SOURCE_FILES})