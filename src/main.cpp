#include <cstdio>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>

#include <GLFW/glfw3.h>
#include <GLES3/gl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <nlohmann/json.hpp>

#include "graphics/texture.h"
#include "graphics/shader.h"
#include "graphics/mesh.h"
#include "graphics/mesh_builder.h"

#define DEBUG
#include "debug.h"

using namespace std;
using json = nlohmann::json;

GLFWwindow* window;

std::unique_ptr<Mesh> mesh;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int modifier) {
    printf("key callback : %d %d\n", key, scancode);
}

void mainLoop() {
    glClear(GL_COLOR_BUFFER_BIT);

    mesh->Draw();

    glfwSwapBuffers(window);
    glfwPollEvents();
}

Shader LoadShader(std::string vertex_shader, std::string fragment_shader) {
    ifstream vertex_shader_file(vertex_shader);
    ifstream fragment_shader_file(fragment_shader);

    stringstream vertex_shader_stream, fragment_shader_stream;
    vertex_shader_stream << vertex_shader_file.rdbuf();
    fragment_shader_stream << fragment_shader_file.rdbuf();
    
    vertex_shader_file.close();
    fragment_shader_file.close();
    
    string vertex_shader_code = vertex_shader_stream.str();
    string fragment_shader_code = fragment_shader_stream.str();

    return Shader(vertex_shader_code.c_str(), fragment_shader_code.c_str());
}

Texture LoadTexture(std::string path) {
    unsigned int texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // or GL_LINEAR_MIPMAP_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int img_width, img_height, img_nr_channels;
    unsigned char *data = stbi_load(path.c_str(), &img_width, &img_height, &img_nr_channels, 0);
    if (!data) {
        DEBUG_STDERR("Texture load failed\n");
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    return Texture(texture_id);
}

int main() {
    json test;
    ifstream json_file("/res/minecraft_atlas.json");
    json_file >> test;
    json_file.close();
    printf("Test : %s\n", test["name_example_1"]["origin"].dump().c_str());

    if (!glfwInit())
        return -1;

    int width = 640;
    int height = 480;
    std::string title = "untitled_game";

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);

    std::vector<Vertex> vertices = {
        Vertex(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)),
        Vertex(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
        Vertex(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
    };
    std::vector<unsigned int> indices = {
        0, 1, 2,
        0, 2, 3,
    };
    MeshProfile mesh_quad(vertices, indices, "");

    MeshProfile mesh_tilemap;
    for (int x = -2; x < 2; x++) {
        for (int y = -2; y < 2; y++) {
            mesh_tilemap.append(mesh_quad
                // .rotate(glm::radians(45.0f), glm::vec3(0, 0, 1))
                // .scale(glm::vec3(0.5f, 1.0f, 1.0f))
                .translate(glm::vec3(x, y, 0)));
        }
    }
    
    mesh = std::make_unique<Mesh>(BuildMesh(mesh_tilemap));
    mesh->Bind();

    Texture tex = LoadTexture("/res/container.jpg");
    tex.Bind();

    Shader shader = LoadShader("/res/shader.vs", "/res/shader.fs");
    shader.Use();

    glm::mat4 proj = glm::ortho(
        -width / 2.0f, width / 2.0f,
        -height / 2.0f, height / 2.0f,
        -0.1f, 100.0f);
    glm::mat4 model(1);
    glm::mat4 view(1);
    glm::mat4 tex_transform(1);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, 0.0f));
    view = glm::scale(view, glm::vec3(100.0f, 100.0f, 100.0f));
    tex_transform = glm::scale(tex_transform, glm::vec3(1.0f, 1.0f, 1.0f));

    shader.SetUniformMat4("projection", proj);
    shader.SetUniformMat4("model", model);
    shader.SetUniformMat4("view", view);
    shader.SetUniformMat4("tex_transform", tex_transform);

#ifdef EMSCRIPTEN
    emscripten_set_main_loop(&mainLoop, 0, 1);
#else
    while (!glfwWindowShouldClose(window)) {
        mainLoop();
    }
#endif

    glfwTerminate();

    return 0;
}