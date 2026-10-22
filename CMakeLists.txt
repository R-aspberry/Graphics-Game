
cmake_minimum_required(VERSION 3.20)

project(TowerDefense LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/glfw)

add_executable(TowerDefense
    src/main.cpp
    src/ui/MainMenu.cpp
    src/glad.c
    src/Shader.cpp
src/Renderer.cpp
src/utils/ResourceManager.cpp
src/Game.cpp
src/Shader.cpp
src/Renderer.cpp
src/utils/ResourceManager.cpp
)

target_include_directories(TowerDefense PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(TowerDefense PRIVATE glfw)
if(WIN32)
    target_link_libraries(TowerDefense PRIVATE opengl32 winmm)
endif()

