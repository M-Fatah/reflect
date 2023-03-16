cmake_minimum_required(VERSION 3.20)

project(reflect)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(REFLECT_IS_MAIN_PROJECT OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(REFLECT_IS_MAIN_PROJECT ON)
endif()

option(REFLECT_BUILD_EXAMPLES "Enable building examples." ${REFLECT_IS_MAIN_PROJECT})
option(REFLECT_BUILD_UNITTEST "Enable building unittest." ${REFLECT_IS_MAIN_PROJECT})

set(CMAKE_PDB_OUTPUT_DIRECTORY     "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")

if(MSVC)
    set(REFLECT_COMPILE_OPTIONS -W4 -WX -std:c++20 -Zc:preprocessor -wd5105)
else()
    set(REFLECT_COMPILE_OPTIONS -Wall -Wextra -pedantic -Werror -std=c++2b)
endif()

if (REFLECT_BUILD_EXAMPLES)
    message(STATUS "Build examples flag is enabled.")
    add_subdirectory(examples)
endif()

if (REFLECT_BUILD_UNITTEST)
    message(STATUS "Build unittest flag is enabled.")
    add_subdirectory(unittest)
endif()