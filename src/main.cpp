#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

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

// nothings/stb
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// nlohmann/json
#include <nlohmann/json.hpp>

// zeux/pugixml
#include <pugixml.hpp>

// freetype2
#include <ft2build.h>
#include FT_FREETYPE_H

#define DEBUG
#include "debug.h"

#include "graphics/texture.h"
#include "graphics/shader.h"
#include "graphics/mesh.h"
#include "graphics/mesh_builder.h"
#include "chunk.h"

using json = nlohmann::json;

// glfw
GLFWwindow* window;

// freetype
FT_Library ft_library;
FT_Face    ft_face;

std::unique_ptr<Shader> shader;
std::unique_ptr<Mesh>   mesh;
std::unique_ptr<Mesh>   quad_mesh;

std::unique_ptr<Texture> test_tex;
std::unique_ptr<Texture> tex;
std::unique_ptr<TextureAtlas> rect_pack_tex;

float camera_x;
float camera_y;
float camera_scale = 100.0f;

// for calculating fps
double previous_time;
int frame_count;
double elapsed_time_sum;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int modifier) {
    // DEBUG_STDOUT("key callback : %d %d\n", key, scancode);
    // DEBUG_STDOUT("camera : %f %f\n", camera_x, camera_y);

    if (key == GLFW_KEY_KP_SUBTRACT) {
        camera_scale /= 1.1;
    } else if (key == GLFW_KEY_KP_ADD) {
        camera_scale *= 1.1;
    } else if (key == GLFW_KEY_G && action == GLFW_RELEASE) {
        for (int i = 0; i < 1; i++) {
            int width = rand() % 50 + 10;
            int height = rand() % 50 + 10;

            unsigned char R = rand();
            unsigned char G = rand();
            unsigned char B = rand();
         
            unsigned char* data = new unsigned char[width * height * 3];
            for (int r = 0; r < height; r++) {
                for (int c = 0; c < width; c++) {
                    data[c*3 + r * width * 3 + 0] = R;
                    data[c*3 + r * width * 3 + 1] = G;
                    data[c*3 + r * width * 3 + 2] = B;
                }
            }

            glm::ivec2 pos = rect_pack_tex->Claim(width, height);
            if (pos.x >= 0 && pos.y >= 0) {
                rect_pack_tex->SetData(pos.x, pos.y, width, height, data);
            } else {
                DEBUG_STDOUT("no more space!\n");
            }

            delete[] data;
        }
        rect_pack_tex->Bake();
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
    DEBUG_STDOUT("mouse : %d %d %d\n", button, action, mods);
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

    glClear(GL_COLOR_BUFFER_BIT);

    float speed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera_x += -speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera_x +=  speed;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera_y +=  speed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera_y += -speed;

    glm::mat4 view(1);
    view = glm::scale(view, glm::vec3(camera_scale, camera_scale, 0));
    view = glm::translate(view, glm::vec3(-camera_x, -camera_y, 0.0f));

    shader->SetUniformMat4("view", view);

    tex->Bind();
    mesh->Bind();
    mesh->Draw();

    //test_tex->Bind();
    rect_pack_tex->Bind();
    quad_mesh->Bind();
    quad_mesh->Draw();

    double end_time = glfwGetTime();
    double elapsed_time = end_time - current_time;
    elapsed_time_sum += elapsed_time;

    glfwSwapBuffers(window);
    glfwPollEvents();
}

Shader LoadShader(std::string vertex_shader, std::string fragment_shader) {
    std::ifstream vertex_shader_file(vertex_shader);
    std::ifstream fragment_shader_file(fragment_shader);

    std::stringstream vertex_shader_stream, fragment_shader_stream;
    vertex_shader_stream << vertex_shader_file.rdbuf();
    fragment_shader_stream << fragment_shader_file.rdbuf();
    
    vertex_shader_file.close();
    fragment_shader_file.close();
    
    std::string vertex_shader_code = vertex_shader_stream.str();
    std::string fragment_shader_code = fragment_shader_stream.str();

    return Shader(vertex_shader_code.c_str(), fragment_shader_code.c_str());
}

Texture LoadTexture(std::string path) {
    int img_width, img_height, img_nr_channels;
    unsigned char *data = stbi_load(path.c_str(), &img_width, &img_height, &img_nr_channels, 0);
    if (!data) {
        DEBUG_STDOUT("Texture load failed\n");
    }
    DEBUG_STDOUT("Texture loaded : %d %d %d\n", img_width, img_height, img_nr_channels);
    DEBUG_STDOUT("    First four bytes : %x %x %x %x\n", data[0], data[1], data[2], data[3]);

    GLenum format = GL_RGB;
    if (img_nr_channels == 3) {
        format = GL_RGB;
    } else if (img_nr_channels == 4) {
        format = GL_RGBA;
    } else {
        DEBUG_STDOUT("Unexpected image channels : %d\n", img_nr_channels);
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, img_nr_channels);

    Texture texture(img_width, img_height, img_nr_channels, data, format, format);

    stbi_image_free(data);

    return texture;
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

    // freetype
    FT_Error error = FT_Init_FreeType(&ft_library);
    if (error) {
        DEBUG_STDOUT("Failed to init freetype\n");
        return -1;
    }

    error = FT_New_Face(
        ft_library,
        "/res/D2Coding-Ver1.3.2-20180524-all.ttc",
        0, // face index
        &ft_face
    );
    if (error == FT_Err_Unknown_File_Format) {
        DEBUG_STDOUT("Unknown font file format.\n");
        return -1;
    } else if (error) {
        DEBUG_STDOUT("Failed to load font file.\n");
        return -1;
    }

    error = FT_Set_Pixel_Sizes(ft_face, 0, 128);
    if (error) {
        DEBUG_STDOUT("Failed to set pixel sizes\n");
        return -1;
    }

    error = FT_Load_Char(ft_face, U'뷁', FT_LOAD_RENDER);
    if (error) {
        DEBUG_STDOUT("Failed to load char\n");
        return -1;
    }

    error = FT_Render_Glyph(ft_face->glyph, FT_RENDER_MODE_NORMAL);
    if (error) {
        DEBUG_STDOUT("Failed to render glyph.\n");
        return -1;
    }

    // start of program
    if (!glfwInit())
        return -1;

    int width = 800;
    int height = 600;
    std::string title = "untitled_game";

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mousebutton_callback);

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

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    test_tex = std::make_unique<Texture>(
        ft_face->glyph->bitmap.width,
        ft_face->glyph->bitmap.rows,
        1,
        ft_face->glyph->bitmap.buffer,
        GL_LUMINANCE,
        GL_LUMINANCE
    );
    DEBUG_STDOUT("Glyph image test : width = %d, height = %d, width bytes = %d\n",
        ft_face->glyph->bitmap.width,
        ft_face->glyph->bitmap.rows,
        ft_face->glyph->bitmap.pitch
    );
    // for (int r = 0; r < ft_face->glyph->bitmap.rows; r++) {
    //     for (int c = 0; c < ft_face->glyph->bitmap.width; c++) {
    //         unsigned char value = ft_face->glyph->bitmap.buffer[c + r * ft_face->glyph->bitmap.width];
    //         DEBUG_STDOUT("%3d, ", value);
    //     }
    //     DEBUG_STDOUT("\n");
    // }
    // DEBUG_STDOUT("\n");
    // for (int r = 0; r < ft_face->glyph->bitmap.rows; r++) {
    //     for (int c = 0; c < ft_face->glyph->bitmap.width; c++) {
    //         unsigned char value = ft_face->glyph->bitmap.buffer[c + r * ft_face->glyph->bitmap.width];
    //         std::string scale = " .:-=+*#%@";
    //         char t = scale[(int) (value / 256.0f * scale.size())];
    //         DEBUG_STDOUT("%c%c", t, t);
    //     }
    //     DEBUG_STDOUT("\n");
    // }

    rect_pack_tex = std::make_unique<TextureAtlas>(
        500, 500, 3,
        nullptr,
        GL_RGB8, GL_RGB
    );

    shader = std::make_unique<Shader>(LoadShader(
        "/res/vert.glsl",
        "/res/frag.glsl"
    ));
    shader->Use();

    glm::mat4 proj = glm::ortho(
        -width / 2.0f, width / 2.0f,
        -height / 2.0f, height / 2.0f,
        -0.1f, 100.0f);
    glm::mat4 model(1);
    glm::mat4 tex_transform(1);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    tex_transform = glm::scale(tex_transform, glm::vec3(1.0f, 1.0f, 1.0f));
    // tex_transform = glm::translate(tex_transform, glm::vec3(4 / 16.0f, 0 / 16.0f, 0.0f));

    shader->SetUniformMat4("projection", proj);
    shader->SetUniformMat4("model", model);
    shader->SetUniformMat4("tex_transform", tex_transform);

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