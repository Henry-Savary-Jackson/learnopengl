cmake_minimum_required(VERSION 3.23)

# set the project name and version
project(Learning-OpenGL VERSION 0.1)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(engine STATIC global_info.c camera.c batch_render.c light.c textures.c shader.c mesh.c object.c render.c)
target_include_directories( engine PUBLIC ../../include)
