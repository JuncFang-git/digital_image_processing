cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)
project(image_process)
find_package(OpenCV REQUIRED)

file(GLOB SRC source/*.cpp)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/glog/lib)

add_executable(main ${SRC})
target_include_directories(main PRIVATE include 3rdparty/glog/include ${OpenCV_INCLUDE_DIRS})
target_link_libraries(main PUBLIC ${OpenCV_LIBS} glog)
