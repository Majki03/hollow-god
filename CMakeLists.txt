cmake_minimum_required(VERSION 3.22)
project(hollow_god LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

include(FetchContent)

# SFML 2.6 is the last stable line before 3.x. It's well-documented and
# available on every platform we care about. Pinning the tag keeps builds
# reproducible for anyone cloning the repo.
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        2.6.1
    GIT_SHALLOW    TRUE
)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SFML)

# Single-header JSON library. Used for data-driven enemy stats, wave tables,
# and boon definitions. Header-only — no compiled output of its own.
FetchContent_Declare(nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
    GIT_SHALLOW    TRUE
)
set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
set(JSON_Install    OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(nlohmann_json)

add_executable(hollow_god
    src/main.cpp
    src/Application.cpp
    src/audio/AudioSystem.cpp
    src/data/DataStore.cpp
    src/render/PostProcess.cpp
    src/entity/weapon/SwordWeapon.cpp
    src/entity/Archer.cpp
    src/entity/Brute.cpp
    src/entity/Charger.cpp
    src/entity/EnemyBase.cpp
    src/entity/Grunt.cpp
    src/entity/Projectile.cpp
    src/hud/Hud.cpp
    src/entity/Player.cpp
    src/input/ActionMap.cpp
    src/input/InputState.cpp
    src/scene/BoonSelectionScene.cpp
    src/scene/DeathScene.cpp
    src/scene/GameScene.cpp
    src/scene/PauseScene.cpp
    src/scene/SceneStack.cpp
    src/scene/VictoryScene.cpp
    src/scene/TitleScene.cpp
    src/world/Room.cpp
    src/world/World.cpp
)

target_include_directories(hollow_god PRIVATE src)

# Dev-only path shortcuts. Both point into the source tree so any checkout
# works without a post-build copy step. Replace with relative paths when bundling.
target_compile_definitions(hollow_god PRIVATE
    HG_DEV_FONT="${sfml_SOURCE_DIR}/examples/shader/resources/tuffy.ttf"
    HG_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets/data"
    HG_SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders"
)

# SFML 2.6 headers trip -Wdeprecated-declarations on newer libc++
# (sf::String instantiates basic_string<uint32_t>). Consuming them as SYSTEM
# keeps our own warnings strict without drowning in upstream noise.
target_include_directories(hollow_god SYSTEM BEFORE PRIVATE
    ${sfml_SOURCE_DIR}/include)

target_link_libraries(hollow_god PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
    nlohmann_json::nlohmann_json
)

if(MSVC)
    target_compile_options(hollow_god PRIVATE /W4)
else()
    target_compile_options(hollow_god PRIVATE -Wall -Wextra -Wpedantic)
endif()
