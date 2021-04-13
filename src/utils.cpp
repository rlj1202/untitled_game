#include "utils.h"

#include <iostream>
#include <fstream>
#include <sstream>

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