cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

find_package(OpenCV REQUIRED)
find_package(PCL 1.7 REQUIRED)

include_directories(${PCL_INCLUDE_DIRS}  
                    ${OpenCV_INCLUDE_DIRS} 
                    ${PROJECT_SOURCE_DIR}
                    /home/zengzhen/Documents) 
add_definitions(${PCL_DEFINITIONS})
link_directories(${PCL_LIBRARIES}  ${OpenCV_LIBS})

add_executable (extract_sec extract_sec.cpp)
target_link_libraries (extract_sec 
                       ${PCL_LIBRARIES} 
                       ${OpenCV_LIBS}
                       /home/zengzhen/Documents/Kinect/lib/libconvert_rgbd_to_image.so
                       table_obj_seg
                       pcd_cloud
                       trackRigid
                       view2D
                       view3D
                       util
                       colorDetector
                       touchDetecor
                       moveDetector
                       mainGraph)
