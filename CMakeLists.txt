cmake_minimum_required(VERSION 3.28)
project(ntbtl)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_image EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_ttf EXCLUDE_FROM_ALL)

add_library(engine STATIC 
    src/engine/game_machine.h
    src/engine/input_machine.h
    src/engine/move_machine.cpp
    src/engine/move_machine.h
    src/engine/player_machine.h
    src/engine/state_machine.h
    src/engine/attack_machine.cpp
    src/engine/attack_machine.h
    src/engine/block_machine.h)

include_directories(src/engine)
add_executable(ntbtl src/front/main.cpp)

target_link_libraries(ntbtl PUBLIC engine)
target_link_libraries(ntbtl PRIVATE SDL3::SDL3)
target_link_libraries(ntbtl PRIVATE SDL3_image::SDL3_image)
target_link_libraries(ntbtl PRIVATE SDL3_ttf::SDL3_ttf)