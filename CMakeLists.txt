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

add_executable(hollow_god
    src/main.cpp
    src/Application.cpp
    src/boon/BoonPool.cpp
    src/entity/Brute.cpp
    src/entity/Charger.cpp
    src/entity/EnemyBase.cpp
    src/entity/Grunt.cpp
    src/hud/Hud.cpp
    src/entity/Player.cpp
    src/input/ActionMap.cpp
    src/input/InputState.cpp
    src/scene/BoonSelectionScene.cpp
    src/scene/DeathScene.cpp
    src/scene/GameScene.cpp
    src/scene/SceneStack.cpp
    src/scene/VictoryScene.cpp
    src/scene/TitleScene.cpp
    src/world/Room.cpp
    src/world/World.cpp
)

target_include_directories(hollow_god PRIVATE src)

# Dev-only convenience: point at a font that ships with SFML's examples so we
# don't need a bundled asset yet. Replace with a proper asset path before shipping.
target_compile_definitions(hollow_god PRIVATE
    HG_DEV_FONT="${sfml_SOURCE_DIR}/examples/shader/resources/tuffy.ttf"
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
)

if(MSVC)
    target_compile_options(hollow_god PRIVATE /W4)
else()
    target_compile_options(hollow_god PRIVATE -Wall -Wextra -Wpedantic)
endif()
