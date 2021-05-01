#include "asset.h"

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

#include "graphics/shader.h"
#include "graphics/texture.h"

/*
 * 
 * 
 * 
 */

template<>
std::unique_ptr<std::ifstream> GetAsset(std::wstring path) {
    return std::move(std::make_unique<std::ifstream>(path));
}

template<>
std::unique_ptr<std::string> GetAsset(std::wstring path) {
    auto stream = GetAsset<std::ifstream>(path);

    std::stringstream string_stream;
    string_stream << stream->rdbuf();

    stream->close();

    std::string result = string_stream.str();

    return std::move(std::make_unique<std::string>(result));
}

template<>
std::unique_ptr<std::wstring> GetAsset(std::wstring path) {
    auto stream = GetAsset<std::ifstream>(path);

    std::wstringstream string_stream;
    string_stream << stream->rdbuf();

    stream->close();

    std::wstring result = string_stream.str();

    return std::move(std::make_unique<std::wstring>(result));
}

template<>
std::unique_ptr<pugi::xml_document> GetAsset(std::wstring path) {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(path.c_str());
    if (!result) {
        DEBUG_STDOUT("Texture Atlas xml file load failed.\n");

        return nullptr;
    }

    return std::move(std::make_unique<pugi::xml_document>(doc));
}

template<>
std::unique_ptr<Shader> GetAsset(std::wstring path) {
    std::unique_ptr<std::string> vertex_shader_code = GetAsset<std::string>(path); // TODO:
    std::unique_ptr<std::string> fragment_shader_code = GetAsset<std::string>(path);

    return std::move(std::make_unique<Shader>(
        vertex_shader_code->c_str(),
        fragment_shader_code->c_str()
    ));
}

template<>
std::unique_ptr<TextureAtlas> GetAsset(std::wstring path) {
    return std::move(std::make_unique<TextureAtlas>());
}
