cmake_minimum_required(VERSION 3.1)

project(test)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-D__STDC_CONSTANT_MACROS")

find_package(OpenCV REQUIRED)

message(status "config:${OpenCV_DIR}")
message(status "version:${OpenCV_VERSION}")
message(status "libraries:${OpenCV_LIBS}")
message(status "include path:${OpenCV_INCLUDE_DIRS}")

add_executable(test opencv_test.cpp)
target_link_libraries(test PRIVATE ${OpenCV_LIBS})

