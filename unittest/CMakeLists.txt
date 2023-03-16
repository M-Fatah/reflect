# External dependencies.
include(FetchContent)
FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG        v2.4.8
    GIT_PROGRESS   TRUE
)
set(DOCTEST_WITH_TESTS OFF)
set(DOCTEST_NO_INSTALL ON)
FetchContent_MakeAvailable(doctest)

set(HEADER_FILES
)

set(SOURCE_FILES
    src/unittest.cpp
    src/unittest_reflect.cpp
)

set(LIBS
    doctest
)

add_executable(unittest ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(unittest ${LIBS})
target_compile_options(unittest PRIVATE ${GLOBAL_COMPILE_OPTIONS})