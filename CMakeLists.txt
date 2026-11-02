
cmake_minimum_required(VERSION 3.20)
project(TowerDefense LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

# Build the reusable game code separately so it can be tested without main.cpp.
list(FILTER PROJECT_SOURCES EXCLUDE REGEX "/main[.]cpp$")
list(FILTER PROJECT_SOURCES EXCLUDE REGEX "/glad[.]c$")

add_library(TowerDefenseCore STATIC
    ${PROJECT_SOURCES}
)

target_include_directories(TowerDefenseCore PUBLIC
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

target_link_libraries(TowerDefenseCore PUBLIC
    glfw
    OpenGL::GL
    glm::glm
)

if(WIN32)
    target_link_libraries(TowerDefenseCore PUBLIC
        opengl32
        winmm
        dwmapi
        gdi32
    )
endif()

add_executable(TowerDefense src/main.cpp)
target_link_libraries(TowerDefense PRIVATE TowerDefenseCore)

# Keep the executable runnable from Explorer or a different working directory.
add_custom_command(TARGET TowerDefense POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:TowerDefense>/assets"
    COMMENT "Copying game assets beside TowerDefense"
)

# MinGW programs need these runtime DLLs when its bin folder is not on PATH.
if(MINGW)
    get_filename_component(compiler_bin "${CMAKE_CXX_COMPILER}" DIRECTORY)
    foreach(runtime_dll libstdc++-6.dll libgcc_s_seh-1.dll libgcc_s_dw2-1.dll libgcc_s_sjlj-1.dll libwinpthread-1.dll)
        set(runtime_path "${compiler_bin}/${runtime_dll}")
        if(EXISTS "${runtime_path}")
            add_custom_command(TARGET TowerDefense POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${runtime_path}" "$<TARGET_FILE_DIR:TowerDefense>"
            )
        endif()
    endforeach()
endif()

set_target_properties(TowerDefense PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Regression tests for the game systems.
# Do not compile src/glad.c here.
option(BUILD_GAME_TESTS "Build game regression tests" ON)

if(BUILD_GAME_TESTS)
    enable_testing()
    add_executable(CombatSystemTests
        tests/CombatSystemTest.cpp
        src/systems/CombatSystem.cpp
        src/systems/CollisionSystem.cpp
        src/systems/GameManager.cpp
        src/systems/WaveManager.cpp
        src/entities/Entity.cpp
        src/entities/Player.cpp
        src/entities/MeleePlayer.cpp
        src/entities/RangedPlayer.cpp
        src/entities/DefensePlayer.cpp
        src/entities/Projectile.cpp
        src/entities/Enemy.cpp
        src/entities/EasyEnemy.cpp
        src/entities/MediumEnemy.cpp
        src/entities/HardEnemy.cpp
        src/entities/EnemyProjectile.cpp
        src/entities/Collectible.cpp
        src/world/Grid.cpp
    )
    target_link_libraries(CombatSystemTests PRIVATE glm::glm)
    target_include_directories(CombatSystemTests PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/systems"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utils"
    )
    add_test(NAME CombatSystemTests COMMAND CombatSystemTests)

    add_executable(GameLogicTests tests/GameLogicTest.cpp)
    target_link_libraries(GameLogicTests PRIVATE TowerDefenseCore)
    add_test(NAME GameLogicTests COMMAND GameLogicTests)
    add_executable(RequirementsTests tests/RequirementsTest.cpp)
    target_link_libraries(RequirementsTests PRIVATE TowerDefenseCore)
    add_test(NAME RequirementsTests COMMAND RequirementsTests)
    # Run manually on a machine with OpenGL; saves frames for visual inspection.
    add_executable(RenderSmokeTest tests/RenderSmokeTest.cpp)
    target_link_libraries(RenderSmokeTest PRIVATE TowerDefenseCore)

    add_executable(EnemyTests
        tests/EnemyTest.cpp
        src/entities/Enemy.cpp
        src/entities/EasyEnemy.cpp
        src/entities/MediumEnemy.cpp
        src/entities/HardEnemy.cpp
    )

    target_link_libraries(EnemyTests PRIVATE glm::glm)
    add_test(NAME EnemyTests COMMAND EnemyTests)

    target_include_directories(EnemyTests PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/entities"
    )

    add_executable(WaveManagerTests
        tests/WaveManagerTest.cpp
        src/systems/WaveManager.cpp
        src/entities/Entity.cpp
        src/entities/Collectible.cpp
        src/entities/EnemyProjectile.cpp
        src/world/Grid.cpp
        src/entities/Enemy.cpp
        src/entities/EasyEnemy.cpp
        src/entities/MediumEnemy.cpp
        src/entities/HardEnemy.cpp
    )

    target_link_libraries(WaveManagerTests PRIVATE glm::glm)
    add_test(NAME WaveManagerTests COMMAND WaveManagerTests)

    target_include_directories(WaveManagerTests PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/entities"
    )
endif()
