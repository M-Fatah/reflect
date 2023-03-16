set(HEADER_FILES
)

set(SOURCE_FILES
    print.cpp
)

set(LIBS
)

add_executable(print ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(print PUBLIC ${LIBS})
target_compile_options(print PRIVATE ${REFLECT_COMPILE_OPTIONS})