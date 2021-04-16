file(GLOB SOURCES
    *.cpp
)

if (EMSCRIPTEN)
    add_executable(test1 ${SOURCES})
    add_test(
        NAME test1
        COMMAND node test1.js
    )
    # target_link_libraries(test PUBLIC)
endif()