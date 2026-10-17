cmake_minimum_required(VERSION 3.20)
project(wish C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Default to a Debug build (with -g) when no build type is given
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug)
endif()

add_executable(wish
        app/wish.c
        app/modes/batch/batch.c
        app/modes/interactive/interactive.c
        app/modes/interactive/builtincmd/builtin_cmd.c
        app/modes/interactive/externalcmd/external_cmd.c
        app/modes/interactive/common/error.c
        app/modes/interactive/common/error.h
        app/path.c
        app/path.h
)

target_compile_options(wish PRIVATE -Wall -Wextra)

target_include_directories(wish PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Put the binary next to the sources so ./test-wish.sh finds ./wish
set_target_properties(wish PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
