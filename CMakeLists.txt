cmake_minimum_required(VERSION 3.7)
project(PPM)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Vec3.h Image.h Material.h Color.h Object.h Camera.h Light.h Obj.h)
add_executable(PPM ${SOURCE_FILES})