cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

SET( CMAKE_CXX_FLAGS_RELEASE "-std=gnu++0x -fPIC -O3" CACHE STRING "" FORCE )
SET( CMAKE_CXX_FLAGS_DEBUG "-g -std=gnu++0x -fPIC -ggdb3" CACHE STRING "" FORCE )

project(sec)
add_subdirectory (TableObjectSegmentation) 
add_subdirectory(Tracker)
add_subdirectory(Visualizer)
add_subdirectory(util)
add_subdirectory(Detector)
add_subdirectory(SEC)

find_package(PCL 1.7 REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(${PCL_INCLUDE_DIRS} 
                    ${OpenCV_INCLUDE_DIRS}
                    /home/zengzhen/Documents/
                    ${PROJECT_SOURCE_DIR})
add_definitions(${PCL_DEFINITIONS})

add_executable (planar_segmentation planar_segmentation.cpp)
target_link_libraries (planar_segmentation 
                       ${PCL_LIBRARIES} 
                       ${OpenCV_LIBS}
                       /home/zengzhen/Documents/Kinect/lib/libconvert_rgbd_to_image.so
                       table_obj_seg
                       pcd_cloud
                       track3D
                       trackRigid
                       view2D
                       view3D
                       util
                       colorDetector
                       touchDetecor
                       moveDetector
                       mainGraph)






