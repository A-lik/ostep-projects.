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
        app/modes/common/cmd_executor/builtin/builtin_cmd.c
        app/modes/common/cmd_executor/external/external_cmd.c
        app/modes/common/error/error.c
        app/modes/common/error/error.h
        app/path.c
        app/path.h
        app/modes/common/tokenizer/tokenizer.c
        app/modes/common/tokenizer/tokenizer.h
        app/modes/common/cmd_runner/cmd_runner.c
        app/modes/common/cmd_runner/cmd_runner.h
        app/modes/common/parser/input_parser/line_parser.c
        app/modes/common/parser/input_parser/line_parser.h
)

target_compile_options(wish PRIVATE -Wall -Wextra)

target_include_directories(wish PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Put the binary next to the sources so ./test-wish.sh finds ./wish
set_target_properties(wish PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
