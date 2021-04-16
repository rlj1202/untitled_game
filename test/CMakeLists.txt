if (EMSCRIPTEN)
    set(TEST_COMMAND "node")
    set(EXTENSION ".js")
else()
    set(TEST_COMMAND "")
    set(EXTENSION "")
endif()

add_executable(test1 "main.cpp")
add_executable(test2 "test2.cpp")
target_link_libraries(test1
    PRIVATE pugixml
)
set_target_properties(test1 test2
    PROPERTIES LINK_FLAGS "\
                          -s NODERAWFS=1 \
                          -lnodefs.js \
                          "
)
add_test(
    NAME test1
    COMMAND ${TEST_COMMAND} test1${EXTENSION}
)
add_test(
    NAME test2
    COMMAND ${TEST_COMMAND} test2${EXTENSION}
)