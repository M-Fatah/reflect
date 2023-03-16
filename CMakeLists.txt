cmake_minimum_required(VERSION 3.20)

project(reflect)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_PDB_OUTPUT_DIRECTORY     "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")

# Global compile options.
if(MSVC)
    set(GLOBAL_COMPILE_OPTIONS -W4 -WX -std:c++20 -Zc:preprocessor -wd5105)
else()
    set(GLOBAL_COMPILE_OPTIONS -Wall -Wextra -pedantic -Werror -std=c++2b)
endif()

add_subdirectory(examples)
add_subdirectory(unittest)