cmake_minimum_required(VERSION 3.20)
project(TowerDefense LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw)
add_subdirectory(external/glm)
find_package(OpenGL REQUIRED)

# Main menu and gameplay use GLAD 2 header-only.  Its implementation is
# compiled in src/main.cpp (GLAD_GL_IMPLEMENTATION), not src/glad.c (GLAD 1).
add_executable(TowerDefense
    src/main.cpp
    src/glad2.cpp
    src/world/Grid.cpp
    src/ui/MainMenu.cpp
    src/Shader.cpp
    src/Renderer.cpp
    src/utils/ResourceManager.cpp
    src/Game.cpp
    src/systems/WaveManager.cpp
    src/entities/Enemy.cpp
    src/entities/EasyEnemy.cpp
    src/entities/MediumEnemy.cpp
    src/entities/HardEnemy.cpp
)

target_include_directories(TowerDefense PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/stb"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include"
)
target_link_libraries(TowerDefense PRIVATE glfw OpenGL::GL glm::glm)
if(WIN32)
    target_link_libraries(TowerDefense PRIVATE winmm dwmapi)
endif()
set_target_properties(TowerDefense PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Regression tests for member 3's work. Do not compile src/glad.c here.
option(BUILD_GAME_TESTS "Build Enemy and WaveManager unit tests" ON)
if(BUILD_GAME_TESTS)
    add_executable(EnemyTests
        tests/EnemyTest.cpp
        src/entities/Enemy.cpp
        src/entities/EasyEnemy.cpp
        src/entities/MediumEnemy.cpp
        src/entities/HardEnemy.cpp
    )
    target_link_libraries(EnemyTests PRIVATE glm::glm)
    target_include_directories(EnemyTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/entities")

    add_executable(WaveManagerTests
        tests/WaveManagerTest.cpp
        src/systems/WaveManager.cpp
        src/entities/Enemy.cpp
        src/entities/EasyEnemy.cpp
        src/entities/MediumEnemy.cpp
        src/entities/HardEnemy.cpp
    )
    target_link_libraries(WaveManagerTests PRIVATE glm::glm)
    target_include_directories(WaveManagerTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/entities")
endif()
