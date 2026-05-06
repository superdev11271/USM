cmake_minimum_required(VERSION 3.10)
project(pose_reader)

find_package(gazebo REQUIRED)
find_package(TBB REQUIRED)

add_executable(pose_reader main.cpp)

target_link_libraries(pose_reader
  ${GAZEBO_LIBRARIES}
  TBB::tbb
)

include_directories(${GAZEBO_INCLUDE_DIRS})
link_directories(${GAZEBO_LIBRARY_DIRS})