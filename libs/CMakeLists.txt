set(ENTT_USE_LIBCPP OFF CACHE BOOL "")

add_subdirectory(json)
add_subdirectory(glm)
add_subdirectory(pugixml)
add_subdirectory(freetype-2.10.4)
add_subdirectory(entt)

################################################################################
# glad
################################################################################
add_library(glad STATIC glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)

################################################################################
# imgui
################################################################################
file(GLOB IMGUI_SOURCES
    imgui/*.cpp
    imgui/backends/*_glfw.cpp
    imgui/backends/*_opengl3.cpp)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC imgui imgui/backends)
if (WIN32)
    target_link_libraries(imgui PUBLIC glad)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DIMGUI_IMPL_OPENGL_LOADER_GLAD")
endif()

################################################################################
# stb
################################################################################
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE stb)

# file(GLOB STB_SOURCES *.h *.c)
# add_library(stb_static STATIC ${STB_SOURCES})
# add_library(stb_shared SHARED ${STB_SOURCES})
# target_link_libraries(stb_static PUBLIC stb)
# target_link_libraries(stb_shared PUBLIC stb)