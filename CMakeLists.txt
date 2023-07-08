cmake_minimum_required(VERSION 3.10)

set(porject_name TinyRenderer)
project(${porject_name})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# head file
#include_directories(.)
# src file
#aux_source_directory(. SrcFiles)
#set(SrcFiles main.cpp model.cpp tgaimage.cpp)

file(GLOB SOURCES *.h *.cpp)

add_executable(${porject_name} ${SOURCES})

#target_link_libraries(${porject_name} ${OpenCV_LIBRARIES})