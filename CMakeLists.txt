cmake_minimum_required(VERSION 3.22)
project(SDL3Training LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# SDL3 is built from source into ~/.local (Ubuntu 24.04 has no libsdl3-dev package).
list(APPEND CMAKE_PREFIX_PATH "$ENV{HOME}/.local")

find_package(SDL3 REQUIRED CONFIG)
find_package(glm QUIET)

add_executable(${PROJECT_NAME}
        main.cpp
        rectangle.cpp
        circle.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)

# libglm-dev (apt) is header-only and lands in /usr/include, so no link step is
# needed there; link the imported target only when a CMake config was found.
if (glm_FOUND AND TARGET glm::glm)
    target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
endif ()
