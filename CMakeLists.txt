cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

project(untitled_game)

if (EMSCRIPTEN)
    message(STATUS "Emscripten")
endif()

if (NOT EMSCRIPTEN)
    find_package(glfw3 REQUIRED)
    find_package(OpenGL REQUIRED)
endif()

enable_testing()

add_subdirectory(libs)
add_subdirectory(test)

file(GLOB SOURCES
    src/*.cpp
    src/graphics/*.cpp
)

if (EMSCRIPTEN)
    file(GLOB_RECURSE PLATFORM_SOURCES src/platforms/*_webgl.cpp)
elseif (linux)
    file(GLOB_RECURSE PLATFORM_SOURCES src/platforms/*_linux.cpp)
elseif (CYGWIN)
elseif (APPLE)
elseif (WIN32)
endif()

set(CMAKE_EXECUTABLE_SUFFIX ".html")

add_executable(${PROJECT_NAME}
    ${SOURCES} ${PLATFORM_SOURCES}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src      # because of template implementation files...
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/libs/glm
)

if (EMSCRIPTEN)
    set_target_properties(${PROJECT_NAME}
        PROPERTIES LINK_FLAGS "\
                              -fexceptions \
                              -s USE_WEBGL2=1 \
                              -s FULL_ES3=1 \
                              -s USE_GLFW=3 \
                              -s WASM=1 \
                              -s ALLOW_MEMORY_GROWTH=1 \
                              -s NO_DISABLE_EXCEPTION_CATCHING \
                              -lopenal \
                              --preload-file ${PROJECT_SOURCE_DIR}/res@/res \
                              "
)
endif()

target_link_libraries(${PROJECT_NAME}
    PRIVATE glm
    PRIVATE stb
    PRIVATE nlohmann_json::nlohmann_json
    PRIVATE pugixml
    PRIVATE freetype
    PRIVATE EnTT
    # PRIVATE nanogui
)

if (NOT EMSCRIPTEN)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE glfw3
        PRIVATE OpenGL::GL
    )
endif()