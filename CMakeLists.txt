
cmake_minimum_required(VERSION 3.23)

# set the project name and version
project(Learning-OpenGL VERSION 0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(src/engine)

add_executable(output
        src/main.c
        src/glad/glad.c
        )

# alternative cmake -DCMAKE_BUILD_TYPE=Debug <path and other arguments>

target_include_directories( output PUBLIC include)
target_link_libraries(output engine glfw GL X11 pthread Xrandr Xi dl cglm m)

# create compile commands for intelli sense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
