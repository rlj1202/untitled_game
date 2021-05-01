#include "utils.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

// pugixml
#include <pugixml.hpp>

// nothings/stb
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define DEBUG
#include "debug.h"

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

std::unique_ptr<TextureAtlas> LoadTextureAtlas(std::string path) {
    std::filesystem::path xml_filepath(path);

    // Open xml file
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(xml_filepath.c_str());
    if (!result) {
        DEBUG_STDOUT("Texture Atlas xml file load failed.\n");
    }

    // Get texture bound informations
    std::vector<TextureBound> bounds;
    for (auto texture : doc.child("TextureAtlas").children("Texture")) {
        auto name = texture.child("Name").child_value();
        auto pos = texture.child("Position").child_value();
        auto size = texture.child("Size").child_value();

        int pos_x, pos_y;
        std::stringstream(pos) >> pos_x >> pos_y;

        int size_width, size_height;
        std::stringstream(size) >> size_width >> size_height;

        DEBUG_STDOUT("\tTexture in atlas : %s (%d, %d) (%d, %d)\n",
            name, pos_x, pos_y, size_width, size_height);

        auto bound = TextureBound(
            std::string(name),
            glm::ivec2(pos_x, pos_y),
            glm::ivec2(size_width, size_height),
            nullptr
        );
        bounds.push_back(bound);
    }

    // Get img file path spectified in xml file
    std::error_code error;
    auto img_filepath = std::filesystem::absolute(xml_filepath.parent_path() / doc.child("TextureAtlas").child("Path").child_value(), error);
    if (error) {
        DEBUG_STDOUT("Cannot find corresponding img file path.\n");
    }
    DEBUG_STDOUT("Loading image : %s\n", img_filepath.c_str());

    // Load img
    int img_width, img_height, img_nr_channels;
    unsigned char *data = stbi_load(img_filepath.c_str(), &img_width, &img_height, &img_nr_channels, 0);
    if (!data) {
        DEBUG_STDOUT("Texture Atlas load failed.\n");
    }
    DEBUG_STDOUT("Texture Atlas loaded : %d %d %d\n", img_width, img_height, img_nr_channels);
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

    // Return atlas object
    std::unique_ptr<TextureAtlas> texture_atlas = std::make_unique<TextureAtlas>(
        img_width, img_height, img_nr_channels,
        data,
        format, format,
        bounds
    );

    stbi_image_free(data);
    
    return std::move(texture_atlas);
}
