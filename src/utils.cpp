#include "utils.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

// pugixml
#include <pugixml.hpp>

#define DEBUG
#include "debug.h"

ShaderProgram LoadShader(std::string vertex_shader, std::string fragment_shader) {
    std::ifstream vertex_shader_file(vertex_shader);
    std::ifstream fragment_shader_file(fragment_shader);

    std::stringstream vertex_shader_stream, fragment_shader_stream;
    vertex_shader_stream << vertex_shader_file.rdbuf();
    fragment_shader_stream << fragment_shader_file.rdbuf();
    
    vertex_shader_file.close();
    fragment_shader_file.close();
    
    std::string vertex_shader_code = vertex_shader_stream.str();
    std::string fragment_shader_code = fragment_shader_stream.str();

    return ShaderProgram(vertex_shader_code.c_str(), fragment_shader_code.c_str());
}
