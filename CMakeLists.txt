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

# The project uses GLAD 2 from glad2.cpp; exclude the older GLAD 1 source.
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

list(FILTER PROJECT_SOURCES EXCLUDE REGEX "/glad[.]c$")

add_executable(TowerDefense
    ${PROJECT_SOURCES}
)

target_include_directories(TowerDefense PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/entities"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/systems"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ui"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utils"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/world"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/stb"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/glm"
)
target_link_libraries(TowerDefense PRIVATE glfw OpenGL::GL glm::glm)
if(WIN32)
    target_link_libraries(TowerDefense PRIVATE
        opengl32
        winmm
        dwmapi
        gdi32
    )
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
