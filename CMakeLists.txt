
cmake_minimum_required(VERSION 3.20)

project(TowerDefense LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =====================================
# GLFW
# =====================================

add_subdirectory(external/glfw)

# =====================================
# MAIN GAME
# =====================================

add_executable(TowerDefense
    src/main.cpp
    src/world/Grid.cpp
    src/world/HomeBase.cpp
    src/ui/MainMenu.cpp
    src/glad.c
    src/Shader.cpp
    src/Renderer.cpp
    src/utils/ResourceManager.cpp
    src/Game.cpp
    src/ui/PlayerCard.cpp
    src/ui/HUD.cpp
    src/systems/Economy.cpp
    src/systems/WaveManager.cpp
    src/entities/Enemy.cpp
    src/entities/EasyEnemy.cpp
    src/entities/MediumEnemy.cpp
    src/entities/HardEnemy.cpp
)

# =====================================
# INCLUDE DIRECTORIES
# =====================================

target_include_directories(TowerDefense PRIVATE
    "${CMAKE_SOURCE_DIR}/external/glad/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/external/glm"
)

# =====================================
# LINK LIBRARIES
# =====================================

target_link_libraries(TowerDefense PRIVATE
    glfw
)

if(WIN32)
    target_link_libraries(TowerDefense PRIVATE
        opengl32
        winmm
        dwmapi
        gdi32
    )
endif()

# =====================================
# OPTIONAL TESTS
# =====================================

option(BUILD_GAME_TESTS "Build game tests" OFF)

if(BUILD_GAME_TESTS)

    find_package(glm CONFIG REQUIRED)

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

    target_link_libraries(WaveManagerTests PRIVATE
        glm::glm
    )

    target_include_directories(WaveManagerTests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/entities
    )

    # EnemyTests will need its own add_executable()
    # before libraries can be linked to it.

endif()
