cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 14)

project(untitled_game)

if (CYGWIN)
    message(STATUS "syswin")
elseif (APPLE)
    message(STATUS "apple")
elseif (WIN32)
    message(STATUS "win32")
else()
    message(STATUS "linux")
endif()

if (EMSCRIPTEN)
    message(STATUS "Emscripten")
endif()

if (NOT EMSCRIPTEN)
    find_package(glfw3 REQUIRED)
    find_package(OpenGL REQUIRED)
endif()

add_subdirectory(libs)

file(GLOB_RECURSE SOURCES
    src/*.cpp
    src/graphics/*.cpp
)

set(CMAKE_EXECUTABLE_SUFFIX ".html")

add_executable(${PROJECT_NAME}
    ${SOURCES}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/libs/glm
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES LINK_FLAGS "\
                          -s USE_WEBGL2=1 \
                          -s FULL_ES3=1 \
                          -s USE_GLFW=3 \
                          -s WASM=1 \
                          -s ALLOW_MEMORY_GROWTH=1 \
                          --preload-file ${PROJECT_SOURCE_DIR}/res@/res \
                          "
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE glm
    PRIVATE stb
    PRIVATE nlohmann_json::nlohmann_json
    PRIVATE pugixml
    PRIVATE freetype
    # PRIVATE nanogui
)

if (NOT EMSCRIPTEN)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE glfw3
        PRIVATE OpenGL::GL
    )
endif()