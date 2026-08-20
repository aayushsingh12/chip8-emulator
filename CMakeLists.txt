#Update path to local vcpkg installation
set(CMAKE_TOOLCHAIN_FILE "C:/Users/Aayush/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

cmake_minimum_required(VERSION 3.10.0)
project(chip8-emulator VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 CONFIG REQUIRED)

add_executable(chip8-emulator main.cpp chip8.cpp platform.cpp)
target_link_libraries(chip8-emulator PRIVATE SDL2::SDL2)
