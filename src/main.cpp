#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <algorithm>
#include <vector>
#include <fstream>
#include <filesystem>

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

// skypjack/entt
#include <entt/entt.hpp>

#define DEBUG
#include "debug.h"

#include "graphics/texture.h"
#include "graphics/shader.h"
#include "graphics/mesh.h"
#include "graphics/mesh_builder.h"
#include "graphics/gui.h"
#include "graphics/model.h"
#include "chunk.h"
#include "utils.h"
#include "perlin.h"
#include "asset.h"
#include "camera.h"

using json = nlohmann::json;

int width = 800;
int height = 600;
std::string title = "untitled_game";

// glfw
GLFWwindow* window;

std::unique_ptr<AssetManager> asset_manager;

std::unique_ptr<ShaderProgram>     shader;
std::unique_ptr<Mesh>       mesh;
std::unique_ptr<Mesh>       quad_mesh;
std::unique_ptr<_dev::Mesh> test_mesh;
// std::unique_ptr<_Mesh<Vbo<float, 3>, Vbo<float, 2>, Vbo<float, 3, 3>>> test_mesh;

std::unique_ptr<Model> test_model;

Texture*      tex;
Texture*      gui_tex;
TextureAtlas* atlas_test;

std::unique_ptr<Canvas>   canvas;
std::unique_ptr<IGuiNode> root_gui;

std::unique_ptr<Chunk> chunk;

std::unique_ptr<BlockType> blocktype_grass;
std::unique_ptr<BlockType> blocktype_wood_top;

Camera world_camera;
Camera gui_camera;

float camera_x;
float camera_y;
float camera_scale = 100.0f;

// for calculating fps
double previous_time;
int frame_count;
double elapsed_time_sum;
double elapsed_time_sum_world;
double elapsed_time_sum_gui;

glm::vec2 prev_cursor_pos;
int prev_cursor_button;

bool recurMouseButtonEvent(IGuiNode* node, glm::vec2 pos, int button, int action, int modifiers) {
    if (node->GetAbsoluteArea().IsIn(pos)) {
        if (node->OnMouseButton(pos, button, action, modifiers)) {
            return true;
        }
    }

    for (auto& child : node->GetChildren()) {
        if (recurMouseButtonEvent(child.get(), pos, button, action, modifiers)) {
            return true;
        }
    }

    return false;
}

bool recurMouseDragEvent(IGuiNode* node, glm::vec2 pos, glm::vec2 rel, int button, int modifiers) {
    if (node->GetAbsoluteArea().IsIn(pos)) {
        if (node->OnMouseDrag(pos, rel, button, modifiers)) {
            return true;
        }
    }

    for (auto& child : node->GetChildren()) {
        if (recurMouseDragEvent(child.get(), pos, rel, button, modifiers)) {
            return true;
        }
    }

    return false;
}

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
    // DEBUG_STDOUT("scroll callback : %f %f\n", x, y);

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

    // DEBUG_STDOUT("mouse : %d %d %d, %f %f\n", button, action, mods, xpos, ypos);

    recurMouseButtonEvent(root_gui.get(), cur_pos, button, action, mods);

    if (action == GLFW_PRESS) {
        prev_cursor_pos = cur_pos;
        prev_cursor_button = button;
    }
}

void cursorpos_callback(GLFWwindow *window, double xpos, double ypos) {
    if (glfwGetMouseButton(window, prev_cursor_button) == GLFW_PRESS) {
        glm::vec2 cur_pos(xpos, ypos);
        glm::vec2 delta = cur_pos - prev_cursor_pos;

        recurMouseDragEvent(root_gui.get(), cur_pos, delta, prev_cursor_button, 0);

        prev_cursor_pos = cur_pos;
    }
}

void renderGui(Canvas& canvas, IGuiNode* node) {
    node->Draw(canvas);
    canvas.Flush();

    for (auto& child : node->GetChildren()) {
        renderGui(canvas, child.get());
    }
}

void beforeLoop() {
    shader->SetUniform("projection", glm::mat4(1));
    shader->SetUniform("model", glm::mat4(1));
    shader->SetUniform("tex_transform", glm::mat4(1));
    shader->SetUniform("view", glm::mat4(1));
}

void mainLoop() {
    double current_time = glfwGetTime();
    frame_count++;

    if (current_time - previous_time >= 1.0) {
        float mem_total = GetMemoryTotal() / 1024.0f / 1024.0f;
        float mem_usage = GetMemoryUsage() / 1024.0f / 1024.0f;

        DEBUG_STDOUT("FPS : %d, avg elapsed time : %.2f ms, mem : %.1f/%.1f MB, world = %.2f ms, gui = %.2f ms\n",
            frame_count,
            elapsed_time_sum / frame_count * 1000.0,
            mem_usage, mem_total,
            elapsed_time_sum_world / frame_count * 1000.0,
            elapsed_time_sum_gui / frame_count * 1000.0
        );

        frame_count = 0;
        previous_time = current_time;
        elapsed_time_sum = 0;
        elapsed_time_sum_world = 0;
        elapsed_time_sum_gui = 0;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float speed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera_x += -speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera_x +=  speed;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera_y +=  speed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera_y += -speed;

    // Render world
    double begin_time_world = glfwGetTime();

    world_camera.SetScale(camera_scale);
    world_camera.SetPosition(glm::vec3(camera_x, camera_y, 0.0f));

    shader->SetUniform("view", world_camera.GetMatrix());

    tex->Bind();
    mesh->Draw();

    SubTexture* sub_texture;
    if ((sub_texture = atlas_test->GetSubTexture("cobble_stone"))) {
        shader->SetUniform("tex_transform", sub_texture->GetTextureTransformationMatrix());
        sub_texture->Bind();

        quad_mesh->Draw();
    }
    shader->SetUniform("tex_transform", glm::mat4(1));

    atlas_test->Bind(); // TODO:
    chunk->GetModel().Draw();

    test_mesh->Draw();

    test_model->Draw();
    
    // End render world
    double end_time_world = glfwGetTime();
    elapsed_time_sum_world += end_time_world - begin_time_world;

    // Render gui
    double begin_time_gui = glfwGetTime();

    shader->SetUniform("view", gui_camera.GetMatrix());

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glEnable(GL_DEPTH_TEST);
    // glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);

    // Draw GUI
    renderGui(*canvas, root_gui.get());
    canvas->Flush();

    glDisable(GL_BLEND);
    // glDisable(GL_DEPTH_TEST);
    // glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);

    // End render gui
    double end_time_gui = glfwGetTime();
    elapsed_time_sum_gui += end_time_gui - begin_time_gui;

    //
    double end_time = glfwGetTime();
    double elapsed_time = end_time - current_time;
    elapsed_time_sum += elapsed_time;

    glfwSwapBuffers(window);
    glfwPollEvents();
}

int main() {
    srand(std::time(nullptr));

    // entt test
    entt::registry registry;
    for (int i = 0; i < 10; i++) {
        const auto entity = registry.create();
        registry.emplace<int>(entity, i);
    }

    auto view = registry.view<int>();
    view.each([](int& test) {
        DEBUG_STDOUT("Entt : %d\n", test);
    });

    // using json test
    // json test;
    // std::ifstream json_file("/res/minecraft_atlas.json");
    // json_file >> test;
    // json_file.close();
    // printf("Test : %s\n", test["name_example_1"]["origin"].dump().c_str());

    // using xml test
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("/res/block_types.xml");
    if (!result) {
        DEBUG_STDOUT("Failed to load xml file\n");
        return -1;
    }
    for (auto block_type : doc.child("BlockTypes").children("BlockType")) {
        auto name = block_type.child("Name").child_value();
        auto tex_path = block_type.child("Texture").child("Path").child_value();
        DEBUG_STDOUT("block type name : %s\n", name);
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

    //
    world_camera = Camera(glm::ortho(
        -width / 2.0f, width / 2.0f,
        -height / 2.0f, height / 2.0f,
        -1.0f, 100.0f), glm::vec3(), camera_scale);
    
    gui_camera = Camera(glm::ortho(
        0.0f, (float) width,
        (float) height, 0.0f,
        -0.1f, 100.0f));

    // Asset testing
    asset_manager = std::make_unique<AssetManager>("/res/");

    // Textures
    tex = asset_manager->GetAsset<Texture>(L"textures/minecraft_atlas/texture");
    gui_tex = asset_manager->GetAsset<Texture>(L"textures/gui");
    atlas_test = asset_manager->GetAsset<TextureAtlas>(L"textures/minecraft_atlas");

    std::vector<std::wstring> texture_asset_list = asset_manager->GetAssetList<Texture>()->GetLoadedAssetList();
    for (auto asset_path : texture_asset_list) {
        DEBUG_STDOUT("Texture Asset : %S\n", asset_path.c_str());
    }
    
    // Meshes
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
    const MeshProfile meshprofile_quad(vertices, indices, atlas_test);

    MeshProfile meshprofile_tilemap;
    int tilemap_size = 128;
    for (int x = -tilemap_size / 2; x < tilemap_size / 2; x++) {
        for (int y = -tilemap_size / 2; y < tilemap_size / 2; y++) {
            int index = (int) ((Perlin(glm::vec2(x / 20.0f, y / 20.0f)) + 1.0f) / 2.0f * 25);

            int tex_x = index % 5;
            int tex_y = index / 5;

            meshprofile_tilemap.Append(meshprofile_quad
                .Clone()
                .TexScale(glm::vec2(1/16.0f, 1/16.0f))
                .TexTranslate(glm::vec2(tex_x/16.0f, tex_y/16.0f))
                .Translate(glm::vec3(x, y, 0))
            );
        }
    }
    mesh = std::make_unique<Mesh>(BuildMesh(meshprofile_tilemap));
    quad_mesh = std::make_unique<Mesh>(BuildMesh(
        meshprofile_quad
            .Clone()
            .Translate(glm::vec3(-0.5f, -0.5f, 0.0f))
            .Scale(glm::vec3(4.5f, 4.5f, 2.0f))
    ));

    {
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

        std::unique_ptr<Buffer<Vertex>> buffer = std::make_unique<Buffer<Vertex>>(
            GL_ARRAY_BUFFER, GL_STATIC_DRAW
        );
        buffer->SetData(vertices);

        std::unique_ptr<Buffer<unsigned int>> indices_buffer = std::make_unique<Buffer<unsigned int>>(
            GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW
        );
        indices_buffer->SetData(indices);

        _dev::BufferLayout buffer_layout = {
            _dev::BufferElement(0, 3, GL_FLOAT, sizeof(float)),
            _dev::BufferElement(1, 2, GL_FLOAT, sizeof(float)),
        };

        test_mesh = std::make_unique<_dev::Mesh>(std::vector<ITexture*>({atlas_test}));
        test_mesh->AttachBuffer(std::move(buffer), buffer_layout);
        test_mesh->AttachElementArrayBuffer(std::move(indices_buffer));
        test_mesh->Bake();
    }

    // Model
    test_model = std::make_unique<Model>();
    {
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
        const MeshProfile profile_quad(vertices, indices, nullptr);

        const MeshProfile profile_minecraft_quad = profile_quad.Clone().TexScale(glm::vec2(1 / 16.0f, 1 / 16.0f)).SetTexture(atlas_test);
        const MeshProfile profile_2 = profile_quad.Clone().SetTexture(atlas_test->GetSubTexture("wood_top"));
        const MeshProfile profile_3 = profile_quad.Clone().SetTexture(atlas_test->GetSubTexture("cobble_stone"));

        test_model->Add(profile_minecraft_quad.Clone().Translate(glm::vec3(-1, 0, 0)));
        test_model->Add(profile_minecraft_quad.Clone().Translate(glm::vec3(-2, 0, 0)));
        test_model->Add(profile_2.Clone().Translate(glm::vec3(-3, 0, 0)));
        test_model->Add(profile_3.Clone().Translate(glm::vec3(-4, 0, 0)));
    }
    test_model->Bake();

    // Chunk and BlockTypes
    blocktype_grass = std::make_unique<BlockType>(
        "grass", atlas_test->GetSubTexture("grass")
    );
    blocktype_wood_top = std::make_unique<BlockType>(
        "wood_top", atlas_test->GetSubTexture("wood_top")
    );
    chunk = std::make_unique<Chunk>();
    for (int r = 0; r < CHUNK_SIZE; r++) {
        for (int c = 0; c < CHUNK_SIZE; c++) {
            int value = std::rand() % 2;
            if (value == 0)
                chunk->SetBlock({c, r}, Block(blocktype_grass.get()));
            else
                chunk->SetBlock({c, r}, Block(blocktype_wood_top.get()));
        }
    }
    chunk->Bake();

    // Gui elements
    canvas = std::make_unique<Canvas>();
    root_gui = std::make_unique<GuiWindow>(GuiWindow(
        GuiArea{
            10, 10,
            500, 200
        },
        gui_tex,
        10
    ));
    root_gui->AppendChild(std::make_unique<GuiText>(GuiText(
        GuiArea{10, 10, 480, 100},
        L"안녕하세요! 한글 렌더링 테스트입니다. Hello, world! 이렇게 글씨를 마구마구 적을 수 있습니다. ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
    )));
    root_gui->AppendChild(std::make_unique<GuiButton>(GuiButton(
        GuiArea{10, 10 + 16 + 60, 200, 30},
        gui_tex, 10
    )));

    shader = std::make_unique<ShaderProgram>(LoadShader(
        "/res/vert.glsl",
        "/res/frag.glsl"
    ));
    shader->Use();

    beforeLoop();

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