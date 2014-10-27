cmake_minimum_required(VERSION 2.8)
project(HashT)

set(HashT_VERSION_MAJOR 0)
set(HashT_VERSION_MINOR 0)
set(HashT_VERSION_PATCH 0)

add_definitions(-Wall -Wextra -pedantic -Wfloat-equal -Wswitch-default
    -Winit-self -Wshadow -Wbad-function-cast -Wcast-align -Wconversion
    -Wlogical-op -Wstrict-prototypes -Wnested-externs -std=gnu99)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    # cmake adds -g automatically
    set(DEBUG ON)
    message(STATUS "Debug mode active")
else()
    set(DEBUG OFF)
endif()

include_directories(include/)

add_library(hasht STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/generic_hash_table.c
    )
