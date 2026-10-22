
cmake_minimum_required(VERSION 3.20)

project(TowerDefense LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw-3.5.1)
add_subdirectory(external/glm)

find_package(OpenGL REQUIRED)

add_executable(TowerDefense
    src/main.cpp
)

target_include_directories(TowerDefense PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(TowerDefense PRIVATE
    glfw
    OpenGL::GL
    glm::glm
    dwmapi
)

#
# =====================================
# Enemy Tests
# =====================================

add_executable(EnemyTests
    tests/EnemyTest.cpp
    src/entities/Enemy.cpp
    src/entities/EasyEnemy.cpp
    src/entities/MediumEnemy.cpp
    src/entities/HardEnemy.cpp
)

target_link_libraries(EnemyTests PRIVATE glm::glm)

target_include_directories(EnemyTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/entities
)


# =====================================
# WaveManager Tests
# =====================================

add_executable(WaveManagerTests
    tests/WaveManagerTest.cpp
    src/systems/WaveManager.cpp
    src/entities/Enemy.cpp
    src/entities/EasyEnemy.cpp
    src/entities/MediumEnemy.cpp
    src/entities/HardEnemy.cpp
)

target_link_libraries(WaveManagerTests PRIVATE glm::glm)

target_include_directories(WaveManagerTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/entities
)

