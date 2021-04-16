#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <algorithm>
#include <vector>
#include <fstream>

// emscripten
#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

// glfw, gles3
#include <GLFW/glfw3.h>
#include <GLES3/gl3.h>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// nlohmann/json
#include <nlohmann/json.hpp>

// zeux/pugixml
#include <pugixml.hpp>

#define DEBUG
#include "debug.h"

#include "graphics/texture.h"
#include "graphics/shader.h"
#include "graphics/mesh.h"
#include "graphics/mesh_builder.h"
#include "graphics/gui.h"
#include "chunk.h"
#include "utils.h"

using json = nlohmann::json;

int width = 800;
int height = 600;
std::string title = "untitled_game";

// glfw
GLFWwindow* window;

std::unique_ptr<Shader> shader;
std::unique_ptr<Mesh>   mesh;
std::unique_ptr<Mesh>   quad_mesh;
// std::unique_ptr<_Mesh<Vbo<float, 3>, Vbo<float, 2>, Vbo<float, 3, 3>>> test_mesh;

std::unique_ptr<Texture> tex;
std::unique_ptr<Texture> gui_tex;

std::unique_ptr<Canvas>   canvas;
std::unique_ptr<IGuiNode> root_gui;

float camera_x;
float camera_y;
float camera_scale = 100.0f;

// for calculating fps
double previous_time;
int frame_count;
double elapsed_time_sum;

glm::vec2 prev_cursor_pos;
int prev_cursor_button;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int modifier) {
    // DEBUG_STDOUT("key callback : %d %d\n", key, scancode);
    // DEBUG_STDOUT("camera : %f %f\n", camera_x, camera_y);

    if (key == GLFW_KEY_KP_SUBTRACT) {
        camera_scale /= 1.1;
    } else if (key == GLFW_KEY_KP_ADD) {
        camera_scale *= 1.1;
    }
}

void scroll_callback(GLFWwindow *window, double x, double y) {
    DEBUG_STDOUT("scroll callback : %f %f\n", x, y);

    if (y > 0) {
        camera_scale /= 1.1;
    } else if (y < 0) {
        camera_scale *= 1.1;
    }
    camera_scale = std::max(camera_scale, 1.0f);
}

void mousebutton_callback(GLFWwindow *window, int button, int action, int mods) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    glm::vec2 cur_pos(xpos, ypos);

    DEBUG_STDOUT("mouse : %d %d %d, %f %f\n", button, action, mods, xpos, ypos);

    if (root_gui->GetArea().IsIn(cur_pos)) {
        root_gui->OnMouseButton(cur_pos, button, action, mods);
    }

    if (action == GLFW_PRESS) {
        prev_cursor_pos = cur_pos;
        prev_cursor_button = button;
    }
}

void cursorpos_callback(GLFWwindow *window, double xpos, double ypos) {
    if (glfwGetMouseButton(window, prev_cursor_button) == GLFW_PRESS) {
        glm::vec2 cur_pos(xpos, ypos);
        glm::vec2 delta = cur_pos - prev_cursor_pos;

        if (root_gui->GetArea().IsIn(cur_pos)) {
            root_gui->OnMouseDrag(cur_pos, delta, prev_cursor_button, 0);
        }

        prev_cursor_pos = cur_pos;
    }
}

void mainLoop() {
    double current_time = glfwGetTime();
    frame_count++;

    if (current_time - previous_time >= 1.0) {
        DEBUG_STDOUT("FPS : %d, avg elapsed time : %.2f ms\n",
            frame_count,
            elapsed_time_sum / frame_count * 1000.0
        );

        frame_count = 0;
        previous_time = current_time;
        elapsed_time_sum = 0;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float speed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera_x += -speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera_x +=  speed;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera_y +=  speed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera_y += -speed;

    // Render world
    glm::mat4 proj = glm::ortho(
        -width / 2.0f, width / 2.0f,
        -height / 2.0f, height / 2.0f,
        -1.0f, 100.0f);
    glm::mat4 model(1);
    glm::mat4 tex_transform(1);
    glm::mat4 view(1);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    tex_transform = glm::scale(tex_transform, glm::vec3(1.0f, 1.0f, 1.0f));
    view = glm::scale(view, glm::vec3(camera_scale, camera_scale, 0));
    view = glm::translate(view, glm::vec3(-camera_x, -camera_y, 0.0f));

    shader->SetUniformMat4("projection", proj);
    shader->SetUniformMat4("model", model);
    shader->SetUniformMat4("tex_transform", tex_transform);
    shader->SetUniformMat4("view", view);

    tex->Bind();
    mesh->Bind();
    mesh->Draw();
    
    // End render world

    // Render gui
    glm::mat4 gui_proj = glm::ortho(
        0.0f, (float) width,
        (float) height, 0.0f,
        -0.1f, 100.0f
    );
    glm::mat4 init_mat(1);

    shader->SetUniformMat4("projection", gui_proj);
    shader->SetUniformMat4("view", init_mat);
    shader->SetUniformMat4("model", init_mat);
    shader->SetUniformMat4("tex_transform", init_mat);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);

    {
        root_gui->Draw(*canvas, GuiArea{0, 0, 0, 0});
    }
    canvas->Flush();

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    // glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);

    // End render gui

    double end_time = glfwGetTime();
    double elapsed_time = end_time - current_time;
    elapsed_time_sum += elapsed_time;

    glfwSwapBuffers(window);
    glfwPollEvents();
}

int main() {
    srand(std::time(nullptr));

    // using json test
    json test;
    std::ifstream json_file("/res/minecraft_atlas.json");
    json_file >> test;
    json_file.close();
    printf("Test : %s\n", test["name_example_1"]["origin"].dump().c_str());

    // using xml test
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("/res/block_types.xml");
    if (!result) {
        DEBUG_STDOUT("Failed to load xml file\n");
        return -1;
    }
    for (pugi::xml_node block_type : doc.child("Config").child("BlockTypes").children("BlockType")) {
        DEBUG_STDOUT("block type name : %s\n", block_type.child("Name").child_value());
    }

    // start of program
    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mousebutton_callback);
    glfwSetCursorPosCallback(window, cursorpos_callback);

    Chunk chunk;

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
    MeshProfile meshprofile_quad(vertices, indices, "");

    MeshProfile meshprofile_tilemap;
    for (int x = -8; x < 8; x++) {
        for (int y = -8; y < 8; y++) {
            int tex_x = std::rand() % 3;
            int tex_y = std::rand() % 3;

            meshprofile_tilemap.Append(meshprofile_quad
                // .rotate(glm::radians(45.0f), glm::vec3(0, 0, 1))
                .TexScale(glm::vec2(1/16.0f, 1/16.0f))
                .TexTranslate(glm::vec2(tex_x/16.0f, tex_y/16.0f))
                .Translate(glm::vec3(x, y, 0)));
        }
    }
    
    mesh = std::make_unique<Mesh>(BuildMesh(meshprofile_tilemap));
    quad_mesh = std::make_unique<Mesh>(BuildMesh(
        meshprofile_quad
            .Translate(glm::vec3(-0.5f, -0.5f, 0.0f))
            .Scale(glm::vec3(4.5f, 4.5f, 2.0f))
    ));

    tex = std::make_unique<Texture>(LoadTexture("/res/minecraft_atlas.png"));
    gui_tex = std::make_unique<Texture>(LoadTexture("/res/gui.png"));

    canvas = std::make_unique<Canvas>();
    root_gui = std::make_unique<GuiSimpleLayout>(GuiSimpleLayout(
        GuiArea{
            10, 10,
            500, 200
        },
        gui_tex.get(),
        16
    ));

    shader = std::make_unique<Shader>(LoadShader(
        "/res/vert.glsl",
        "/res/frag.glsl"
    ));
    shader->Use();

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