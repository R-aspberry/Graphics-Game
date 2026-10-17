cmake_minimum_required(VERSION 3.20)

project(TowerDefense LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 3.5 REQUIRED)

add_executable(TowerDefense
    src/main.cpp
)

target_link_libraries(TowerDefense PRIVATE glfw)

if(APPLE)
    target_link_libraries(TowerDefense PRIVATE
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
endif()