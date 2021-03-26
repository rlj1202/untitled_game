cmake_minimum_required(VERSION 3.5)

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

# find_package(glfw3 REQUIRED)
# find_package(OpenGL REQUIRED)
# find_package(glm REQUIRED HINTS libs)

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
                          --preload-file ${PROJECT_SOURCE_DIR}/res@/res \
                          "
)

target_link_libraries(${PROJECT_NAME} PRIVATE glm)
target_link_libraries(${PROJECT_NAME} PRIVATE stb)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
# target_link_libraries(${PROJECT_NAME} glfw3)
# target_link_libraries(${PROJECT_NAME} OpenGL::GL)