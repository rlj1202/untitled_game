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

// gles3
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

// ocornut/imgui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// ???
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

// luvoasi
#define LUVOASI_DEBUG_ENABLE
#include "luvoasi/core/base.h"
#include "luvoasi/window.h"
#include "platforms/luvoasi/window_glfw.h"
#include "luvoasi/graphics/shader.h"
#include "luvoasi/graphics/texture.h"
#include "luvoasi/graphics/buffer.h"
#include "luvoasi/graphics/vertex_array.h"
#include "luvoasi/gui/font.h"
#include "luvoasi/rendering/renderer.h"

using json = nlohmann::json;

int width = 800;
int height = 600;
std::string title = "untitled_game";

std::unique_ptr<AssetManager> asset_manager;

std::unique_ptr<Mesh> mesh;
std::unique_ptr<Mesh> quad_mesh;

std::unique_ptr<Model> test_model;
std::unique_ptr<Model> character_model;

Texture*      tex;
Texture*      gui_tex;
TextureAtlas* atlas_test;
Texture*      character_test_tex;

std::unique_ptr<Canvas>   canvas;
std::unique_ptr<IGuiNode> root_gui;

std::unique_ptr<Chunk> chunk;

std::unique_ptr<BlockType> blocktype_grass;
std::unique_ptr<BlockType> blocktype_wood_top;

std::unique_ptr<Luvoasi::Window> luvoasi_window;
std::unique_ptr<Luvoasi::ShaderProgram> luvoasi_shader_program;
std::unique_ptr<Luvoasi::Texture2D> luvoasi_texture_test;
std::unique_ptr<Luvoasi::VertexArray> luvoasi_va_test;
std::unique_ptr<Luvoasi::VertexArray> luvoasi_va_chunk;
std::unique_ptr<Luvoasi::FontRenderer> luvoasi_font_renderer;

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

int fps;
float mem_total;
float mem_usage;
float avg_elapsed_time;
float avg_world_time;
float avg_gui_time;

glm::vec2 prev_cursor_pos;
Luvoasi::MouseButton prev_cursor_button;

bool recurMouseButtonEvent(IGuiNode* node, glm::vec2 pos, Luvoasi::MouseButton button, Luvoasi::KeyState action, int modifiers) {
    if (node->GetAbsoluteArea().IsIn(pos)) {
        if (node->OnMouseButton(pos, (int) button, (int) action, modifiers)) {
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

bool recurMouseDragEvent(IGuiNode* node, glm::vec2 pos, glm::vec2 rel, Luvoasi::MouseButton button, int modifiers) {
    if (node->GetAbsoluteArea().IsIn(pos)) {
        if (node->OnMouseDrag(pos, rel, (int) button, modifiers)) {
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

bool key_callback(Luvoasi::KeyEvent& event) {
    // LUVOASI_DEBUG_STDOUT("key callback : %d %d\n", key, scancode);
    // LUVOASI_DEBUG_STDOUT("camera : %f %f\n", camera_x, camera_y);

    if (event.GetKey() == Luvoasi::Key::KP_SUBTRACT) {
        camera_scale /= 1.1;
    } else if (event.GetKey() == Luvoasi::Key::KP_ADD) {
        camera_scale *= 1.1;
    }

    return true;
}

bool scroll_callback(Luvoasi::ScrollEvent& event) {
    // LUVOASI_DEBUG_STDOUT("scroll callback : %f %f\n", x, y);

    if (event.GetYOffset() > 0) {
        camera_scale /= 1.1;
    } else if (event.GetYOffset() < 0) {
        camera_scale *= 1.1;
    }
    camera_scale = std::max(camera_scale, 1.0f);

    return true;
}

bool mousebutton_callback(Luvoasi::MouseButtonEvent& event) {
    double xpos, ypos;
    event.GetWindow()->GetCursorPos(&xpos, &ypos);
    glm::vec2 cur_pos(xpos, ypos);

    // LUVOASI_DEBUG_STDOUT("mouse : %d %d %d, %f %f\n", button, action, mods, xpos, ypos);

    recurMouseButtonEvent(root_gui.get(), cur_pos, event.GetButton(), event.GetAction(), event.GetModifiers());

    if (event.GetAction() == Luvoasi::KeyState::PRESS) {
        prev_cursor_pos = cur_pos;
        prev_cursor_button = event.GetButton();
    }

    if (event.GetButton() == Luvoasi::MouseButton::BUTTON1 && event.GetAction() == Luvoasi::KeyState::RELEASE) {
        glm::vec3 homogeneous_coord(
            xpos * 2.0f / width - 1.0f,
            -(ypos * 2.0f / height - 1.0f),
            1.0f);
        glm::vec3 world_coord = world_camera.GetWorldCoords(homogeneous_coord);

        LUVOASI_DEBUG_STDOUT("mouse rel : %f %f\n", xpos, ypos);
        LUVOASI_DEBUG_STDOUT("homogenous coord : %f %f\n", homogeneous_coord.x, homogeneous_coord.y);
        LUVOASI_DEBUG_STDOUT("world coord : %f %f\n", world_coord.x, world_coord.y);
    }

    return true;
}

bool cursorpos_callback(Luvoasi::CursorPosEvent& event) {
    if (event.GetWindow()->GetMouseButton(prev_cursor_button) == Luvoasi::KeyState::PRESS) {
        glm::vec2 cur_pos(event.GetX(), event.GetY());
        glm::vec2 delta = cur_pos - prev_cursor_pos;

        recurMouseDragEvent(root_gui.get(), cur_pos, delta, prev_cursor_button, 0);

        prev_cursor_pos = cur_pos;
    }

    return true;
}

void renderGui(Canvas& canvas, IGuiNode* node) {
    node->Draw(canvas);
    canvas.Flush();

    for (auto& child : node->GetChildren()) {
        renderGui(canvas, child.get());
    }
}

void beforeLoop() {
    luvoasi_shader_program->SetUniform("projection", glm::mat4(1));
    luvoasi_shader_program->SetUniform("model", glm::mat4(1));
    luvoasi_shader_program->SetUniform("tex_transform", glm::mat4(1));
    luvoasi_shader_program->SetUniform("view", glm::mat4(1));
}

void mainLoop() {
    double current_time = luvoasi_window->GetTime();
    frame_count++;

    if (current_time - previous_time >= 1.0) {
        fps = frame_count;
        mem_total = GetMemoryTotal() / 1024.0f / 1024.0f;
        mem_usage = GetMemoryUsage() / 1024.0f / 1024.0f;
        avg_elapsed_time = elapsed_time_sum / frame_count * 1000.0;
        avg_world_time = elapsed_time_sum_world / frame_count * 1000.0;
        avg_gui_time = elapsed_time_sum_gui / frame_count * 1000.0;

        frame_count = 0;
        previous_time = current_time;
        elapsed_time_sum = 0;
        elapsed_time_sum_world = 0;
        elapsed_time_sum_gui = 0;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // imgui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    float speed = 0.05f;
    if (luvoasi_window->GetKey(Luvoasi::Key::A) == Luvoasi::KeyState::PRESS) camera_x += -speed;
    if (luvoasi_window->GetKey(Luvoasi::Key::D) == Luvoasi::KeyState::PRESS) camera_x +=  speed;
    if (luvoasi_window->GetKey(Luvoasi::Key::W) == Luvoasi::KeyState::PRESS) camera_y +=  speed;
    if (luvoasi_window->GetKey(Luvoasi::Key::S) == Luvoasi::KeyState::PRESS) camera_y += -speed;

    // Render world
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glEnable(GL_DEPTH_TEST);
    // glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);

    double begin_time_world = luvoasi_window->GetTime();

    world_camera.SetScale(camera_scale);
    world_camera.SetPosition(glm::vec3(camera_x, camera_y, 0.0f));

    luvoasi_shader_program->SetUniform("view", world_camera.GetMatrix());

    // tex->Bind();
    // mesh->Draw();

    // SubTexture* sub_texture;
    // if ((sub_texture = atlas_test->GetSubTexture("cobble_stone"))) {
    //     luvoasi_shader_program->SetUniform("tex_transform", sub_texture->GetTextureTransformationMatrix());
    //     sub_texture->Bind();

    //     quad_mesh->Draw();
    // }
    // luvoasi_shader_program->SetUniform("tex_transform", glm::mat4(1));

    // atlas_test->Bind(); // TODO:
    // chunk->GetModel().Draw();

    // test_mesh->Draw();

    // test_model->Draw();

    atlas_test->Bind();
    luvoasi_va_chunk->Draw();

    // character_test_tex->Bind(); // TODO:
    // luvoasi_texture_test->Bind();
    // character_model->Draw();

    luvoasi_font_renderer->GetBitmapFont()->GetTexture(0)->Bind();
    luvoasi_va_test->Draw();
    
    // End render world
    double end_time_world = luvoasi_window->GetTime();
    elapsed_time_sum_world += end_time_world - begin_time_world;

    // Render gui
    double begin_time_gui = luvoasi_window->GetTime();

    luvoasi_shader_program->SetUniform("view", gui_camera.GetMatrix());
    
    wchar_t buffer[1024];
    int len = std::swprintf(
        buffer, sizeof(buffer),
        L"FPS: %d, avg elapsed time : %.2f ms, mem : %.1f/%.1f MB, world = %.2f ms, gui = %.2f ms",
        fps, avg_elapsed_time, mem_usage, mem_total, avg_world_time, avg_gui_time);
    std::wstring debug_text(buffer, 0, len);
    luvoasi_font_renderer->DrawString(debug_text, 10, 10 + 16);
    luvoasi_font_renderer->DrawString(L"안녕! 루보아시!", 10, 10 + 16*2 + 2*1);
    luvoasi_font_renderer->DrawString(L"STRESS TEST STRESS TEST STRESS TEST STRESS TEST STRESS TEST STRESS TEST STRESS TEST STRESS TEST ", 10, 10 + 16*3 + 2*2);
    luvoasi_font_renderer->DrawString(L"STRESS TEST STRESS TEST STRESS TEST STRESS TEST STRESS TEST STRESS TEST STRESS TEST STRESS TEST ", 10, 10 + 16*4 + 2*3);
    luvoasi_font_renderer->DrawString(L"STRESS TEST STRESS TEST STRESS TEST STRESS TEST STRESS TEST STRESS TEST STRESS TEST STRESS TEST ", 10, 10 + 16*5 + 2*4);
    luvoasi_font_renderer->DrawString(L"沈智洙 ひらがな", 10, 10 + 16*6 + 2*5);
    luvoasi_font_renderer->DrawString(L"이 폰트 렌더링 시스템은 글자를 필요시 동적으로 텍스처에 로드합니다.", 10, 10 + 16*7 + 2*6);
    luvoasi_font_renderer->Render();

    // Draw GUI
    renderGui(*canvas, root_gui.get());
    canvas->Flush();

    glDisable(GL_BLEND);
    // glDisable(GL_DEPTH_TEST);
    // glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);

    // End render gui
    double end_time_gui = luvoasi_window->GetTime();
    elapsed_time_sum_gui += end_time_gui - begin_time_gui;

    // imgui
    {
        ImGui::Begin("Hello, world!");

        ImGui::Text("This is somme useful text.");
        ImGui::Text(u8"한쿡말도 됨?");
        ImGui::SameLine();

        ImGui::End();
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // End frame
    double end_time = luvoasi_window->GetTime();
    double elapsed_time = end_time - current_time;
    elapsed_time_sum += elapsed_time;

    luvoasi_window->SwapBuffers();
    luvoasi_window->PollEvents();
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
        LUVOASI_DEBUG_STDOUT("Entt : %d\n", test);
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
        LUVOASI_DEBUG_STDOUT("Failed to load xml file\n");
        return -1;
    }
    for (auto block_type : doc.child("BlockTypes").children("BlockType")) {
        auto name = block_type.child("Name").child_value();
        auto tex_path = block_type.child("Texture").child("Path").child_value();
        LUVOASI_DEBUG_STDOUT("block type name : %s\n", name);
    }

    // start of program
    luvoasi_window = std::move(Luvoasi::CreateWindow(width, height, title.c_str()));
    assert(luvoasi_window);

    luvoasi_window->Bind();
    luvoasi_window->AddKeyEventHandler(key_callback);
    luvoasi_window->AddScrollEventHandler(scroll_callback);
    luvoasi_window->AddMouseButtonEventHandler(mousebutton_callback);
    luvoasi_window->AddCursorPosEventHandler(cursorpos_callback);

    // imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& imgui_io = ImGui::GetIO();

    ImGui::StyleColorsDark();

    Luvoasi::GLFWWindow* luvoasi_glfw_window = dynamic_cast<Luvoasi::GLFWWindow*>(luvoasi_window.get());
    ImGui_ImplGlfw_InitForOpenGL(luvoasi_glfw_window->GetRawPointer(), true);
    ImGui_ImplOpenGL3_Init("#version 300 es");

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
    character_test_tex = asset_manager->GetAsset<Texture>(L"textures/character_test");

    luvoasi_texture_test = Luvoasi::Texture2D::CreateTexture2DFromFile("/res/textures/character_test.png");

    assert(tex);
    assert(gui_tex);
    assert(atlas_test);
    assert(character_test_tex);

    std::vector<std::wstring> texture_asset_list = asset_manager->GetAssetList<Texture>()->GetLoadedAssetList();
    for (auto asset_path : texture_asset_list) {
        LUVOASI_DEBUG_STDOUT("Texture Asset : %S\n", asset_path.c_str());
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
    // mesh = std::make_unique<Mesh>(BuildMesh(meshprofile_tilemap));
    quad_mesh = std::make_unique<Mesh>(BuildMesh(
        meshprofile_quad
            .Clone()
            .Translate(glm::vec3(-0.5f, -0.5f, 0.0f))
            .Scale(glm::vec3(4.5f, 4.5f, 2.0f))
    ));

    { // NOTE: luvoasi bake chunk
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        unsigned int last_vertex = 0;
        int tilemap_size = 128;
        for (int x = -tilemap_size / 2; x < tilemap_size / 2; x++) {
            for (int y = -tilemap_size / 2; y < tilemap_size / 2; y++) {
                int index = (int) ((Perlin(glm::vec2(x / 20.0f, y / 20.0f)) + 1.0f) / 2.0f * 25);

                int tex_x = index % 5 + 5;
                int tex_y = index / 5 + 5;
                
                vertices.insert(vertices.end(), {
                    x + 0.0f, y + 0.0f, 0.0f,
                    (tex_x + 0.0f) / 16.0f, (tex_y + 1.0f) / 16.0f,

                    x + 1.0f, y + 0.0f, 0.0f,
                    (tex_x + 1.0f) / 16.0f, (tex_y + 1.0f) / 16.0f,

                    x + 1.0f, y + 1.0f, 0.0f,
                    (tex_x + 1.0f) / 16.0f, (tex_y + 0.0f) / 16.0f,

                    x + 0.0f, y + 1.0f, 0.0f,
                    (tex_x + 0.0f) / 16.0f, (tex_y + 0.0f) / 16.0f
                });
                indices.insert(indices.end(), {
                    last_vertex + 0, last_vertex + 1, last_vertex + 2,
                    last_vertex + 0, last_vertex + 2, last_vertex + 3
                });

                last_vertex += 4;
            }
        }

        Luvoasi::BufferLayout layout = {
            // vec3 position
            Luvoasi::BufferElement{ 0, 3, false, Luvoasi::DataType::FLOAT32 },
            // vec2 tex coord
            Luvoasi::BufferElement{ 1, 2, false, Luvoasi::DataType::FLOAT32 },
        };

        auto array_buffer = Luvoasi::ArrayBuffer::CreateBuffer(vertices.data(), vertices.size());
        auto index_buffer = Luvoasi::IndexBuffer::CreateBuffer(indices.data(), indices.size());
        
        luvoasi_va_chunk = Luvoasi::VertexArray::CreateVertexArray();
        luvoasi_va_chunk->AttachArrayBuffer(std::move(array_buffer), layout);
        luvoasi_va_chunk->AttachIndexBuffer(std::move(index_buffer));
        luvoasi_va_chunk->Unbind();
    }
    { // NOTE: luvoasi
        float x_offset = 2.5f;
        float y_offset = 0.3f;
        float size = 20.0f;

        float vertices[] = {
            x_offset + 0.0f * size, y_offset + 0.0f * size, 0.0f, 0.0f, 1.0f,
            x_offset + 1.0f * size, y_offset + 0.0f * size, 0.0f, 1.0f, 1.0f,
            x_offset + 1.0f * size, y_offset + 1.0f * size, 0.0f, 1.0f, 0.0f,
            x_offset + 0.0f * size, y_offset + 1.0f * size, 0.0f, 0.0f, 0.0f,
        };
        unsigned int indices[] = {
            0, 1, 2,
            0, 2, 3,
        };

        Luvoasi::BufferLayout layout = {
            // vec3 position
            Luvoasi::BufferElement{ 0, 3, false, Luvoasi::DataType::FLOAT32 },
            // vec2 tex coord
            Luvoasi::BufferElement{ 1, 2, false, Luvoasi::DataType::FLOAT32 },
        };

        auto array_buffer = Luvoasi::ArrayBuffer::CreateBuffer(vertices, std::size(vertices));
        auto index_buffer = Luvoasi::IndexBuffer::CreateBuffer(indices, std::size(indices));

        luvoasi_va_test = Luvoasi::VertexArray::CreateVertexArray();
        luvoasi_va_test->AttachArrayBuffer(std::move(array_buffer), layout);
        luvoasi_va_test->AttachIndexBuffer(std::move(index_buffer));
        luvoasi_va_test->Unbind();
    }

    // Model
    const MeshProfile profile_quad(
        std::vector<Vertex>({
            Vertex(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)),
            Vertex(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
            Vertex(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
            Vertex(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
        }),
        std::vector<unsigned int>({
            0, 1, 2,
            0, 2, 3,
        }),
        nullptr);

    test_model = std::make_unique<Model>();
    {
        const MeshProfile profile_minecraft_quad = profile_quad.Clone().TexScale(glm::vec2(1 / 16.0f, 1 / 16.0f)).SetTexture(atlas_test);
        const MeshProfile profile_2 = profile_quad.Clone().SetTexture(atlas_test->GetSubTexture("wood_top"));
        const MeshProfile profile_3 = profile_quad.Clone().SetTexture(atlas_test->GetSubTexture("cobble_stone"));

        test_model->Add(profile_minecraft_quad.Clone().Translate(glm::vec3(-1, 0, 0)));
        test_model->Add(profile_minecraft_quad.Clone().Translate(glm::vec3(-2, 0, 0)));
        test_model->Add(profile_2.Clone().Translate(glm::vec3(-3, 0, 0)));
        test_model->Add(profile_3.Clone().Translate(glm::vec3(-4, 0, 0)));
    }
    test_model->Bake();

    character_model = std::make_unique<Model>();
    character_model->Add(profile_quad.Clone().SetTexture(character_test_tex));
    character_model->Bake();

    // Chunk and BlockTypes
    blocktype_grass = std::make_unique<BlockType>(
        "grass", atlas_test->GetSubTexture("grass")
    );
    blocktype_wood_top = std::make_unique<BlockType>(
        "wood_top", atlas_test->GetSubTexture("wood_top")
    );
    // chunk = std::make_unique<Chunk>();
    // for (int r = 0; r < CHUNK_SIZE; r++) {
    //     for (int c = 0; c < CHUNK_SIZE; c++) {
    //         int value = std::rand() % 2;
    //         if (value == 0)
    //             chunk->SetBlock({c, r}, Block(blocktype_grass.get()));
    //         else
    //             chunk->SetBlock({c, r}, Block(blocktype_wood_top.get()));
    //     }
    // }
    // chunk->Bake();

    // Gui elements
    // NOTE: luvoasi
    luvoasi_font_renderer = std::make_unique<Luvoasi::FontRenderer>();

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

    // shaders
    luvoasi_shader_program = Luvoasi::ShaderProgram::CreateShaderProgramFromFiles(
        "/res/vert.glsl",
        "/res/frag.glsl"
    );
    luvoasi_shader_program->Bind();

    beforeLoop();

#ifdef EMSCRIPTEN
    emscripten_set_main_loop(&mainLoop, 0, 1);
#else
    while (!luvoasi_window->ShouldClose()) {
        mainLoop();
    }
#endif

    // imgui cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}