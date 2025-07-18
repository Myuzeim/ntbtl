cmake_minimum_required(VERSION 3.28)
project(ntbtl)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Make sure you have a folder called vendored
# Put source code for SDL there (check github)
# or run:
# git clone https://github.com/libsdl-org/SDL.git
# git clone https://github.com/libsdl-org/SDL_image.git
# git clone https://github.com/libsdl-org/SDL_ttf.git

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_image EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_ttf EXCLUDE_FROM_ALL)

##################################
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

##################################

# Make sure you also have boost 1.82.0 in vendored
# can find here https://www.boost.org/releases/1.82.0/

add_library(net STATIC 
    src/net/net_types.h
    src/net/net_types.cpp
    src/net/history.h 
    src/net/history.cpp)
target_include_directories(net PUBLIC vendored/boost_1_82_0/boost/asio)

##################################

add_executable(net_test src/net/test.cpp)
target_include_directories(net PUBLIC vendored/boost_1_82_0/boost/asio)
target_link_libraries(net_test PUBLIC net)

include_directories(src/engine)
add_executable(ntbtl src/front/main.cpp
    src/front/key_reader.h 
    src/front/key_writer.h)

target_link_libraries(ntbtl PUBLIC engine)
target_link_libraries(ntbtl PRIVATE SDL3::SDL3)
target_link_libraries(ntbtl PRIVATE SDL3_image::SDL3_image)
target_link_libraries(ntbtl PRIVATE SDL3_ttf::SDL3_ttf)

##################################