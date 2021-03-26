add_subdirectory(json)
add_subdirectory(glm)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE stb)

# file(GLOB STB_SOURCES *.h *.c)
# add_library(stb_static STATIC ${STB_SOURCES})
# add_library(stb_shared SHARED ${STB_SOURCES})
# target_link_libraries(stb_static PUBLIC stb)
# target_link_libraries(stb_shared PUBLIC stb)