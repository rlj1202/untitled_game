#include "asset.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

// pugixml
#include <pugixml.hpp>

// nothings/stb
// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define DEBUG
#include "debug.h"

#include "graphics/shader.h"
#include "graphics/texture.h"

template<typename T>
AssetList<T>::AssetList() {

}

template<typename T>
void AssetList<T>::SetAsset(const std::wstring asset_path, std::unique_ptr<T> asset) {
    assets[asset_path] = std::move(asset);
}

template<typename T>
T* AssetList<T>::GetAsset(const std::wstring asset_path) {
    if (assets.count(asset_path) == 0) {
        return nullptr;
    }
    return assets[asset_path].get();
}

template<typename T>
std::vector<std::wstring> AssetList<T>::GetLoadedAssetList() const {
    std::vector<std::wstring> result;

    for (auto& pair : assets) {
        result.push_back(pair.first);
    }

    return result;
}

AssetManager::AssetManager(std::filesystem::path root_dir)
    : root_dir(root_dir) {

}

std::filesystem::path AssetManager::ResolveAssetPath(const std::wstring path) const {
    std::filesystem::path full_path = root_dir;
    full_path /= path;
    full_path = full_path.lexically_normal();

    std::error_code error;
    for (auto& p : std::filesystem::directory_iterator(full_path.parent_path(), error)) {
        std::filesystem::path file_path = p.path();
        file_path.replace_extension("");

        if (file_path.string().rfind(full_path.string()) == 0) {
            DEBUG_STDOUT("ResolveAssetPath() : found! : %s\n", p.path().string().c_str());

            return p.path();
        }
    }
    if (error) {
        DEBUG_STDOUT("ResolveAssetPath() : error : %s\n", error.message().c_str());

        return "";
    }

    return "";
}

template<typename T>
void AssetManager::SetAsset(const std::wstring asset_path, std::unique_ptr<T> asset) {
    AssetList<T>* asset_list = GetAssetList<T>();
    asset_list->SetAsset(asset_path, std::move(asset));
}

template<typename T>
T* AssetManager::GetAsset(const std::wstring asset_path) {
    AssetList<T>* asset_list = GetAssetList<T>();
    if (asset_list->GetAsset(asset_path)) {
        return asset_list->GetAsset(asset_path);
    }

    std::filesystem::path file_path = ResolveAssetPath(asset_path);
    if (std::filesystem::is_directory(file_path)) {
        file_path /= ASSET_FILE_NAME;

        if (!std::filesystem::exists(file_path)) {
            DEBUG_STDOUT("AssetManager::GetAsset() : %s does not exist.\n", file_path.c_str());
            return nullptr;
        }
    }

    std::unique_ptr<T> result = LoadAsset<T>(file_path);
    if (!result) {
        DEBUG_STDOUT("AssetManager::GetAsset() : Failed to load asset file %s.\n", file_path.c_str());
        return nullptr;
    }

    DEBUG_STDOUT("AssetManager::GetAsset() : Succeed to load asset %s!\n", file_path.c_str());

    T* ptr = result.get();
    asset_list->SetAsset(asset_path, std::move(result));

    return ptr;
}

template<typename T>
AssetList<T>* AssetManager::GetAssetList() {
    if (asset_lists.count(typeid(T)) == 0) {
        asset_lists[typeid(T)] = std::move(std::make_unique<AssetList<T>>());
    }
    AssetList<T>* result = dynamic_cast<AssetList<T>*>(asset_lists[typeid(T)].get());

    return result;
}

template<typename T>
std::unique_ptr<T> LoadAsset(std::filesystem::path path) {
    return nullptr;
}

template<>
std::unique_ptr<std::ifstream> LoadAsset(std::filesystem::path path) {
    return std::move(std::make_unique<std::ifstream>(path));
}

template<>
std::unique_ptr<std::string> LoadAsset(std::filesystem::path path) {
    auto stream = LoadAsset<std::ifstream>(path);

    std::stringstream string_stream;
    string_stream << stream->rdbuf();

    stream->close();

    std::string result = string_stream.str();

    return std::move(std::make_unique<std::string>(result));
}

template<>
std::unique_ptr<std::wstring> LoadAsset(std::filesystem::path path) {
    auto stream = LoadAsset<std::ifstream>(path);

    std::wstringstream string_stream;
    string_stream << stream->rdbuf();

    stream->close();

    std::wstring result = string_stream.str();

    return std::move(std::make_unique<std::wstring>(result));
}

template<>
std::unique_ptr<pugi::xml_document> LoadAsset(std::filesystem::path path) {
    std::unique_ptr<pugi::xml_document> doc = std::make_unique<pugi::xml_document>();
    pugi::xml_parse_result result = doc->load_file(path.c_str());
    if (!result) {
        DEBUG_STDOUT("Texture Atlas xml file load failed.\n");

        return nullptr;
    }

    return std::move(doc);
}

template<>
std::unique_ptr<Texture> LoadAsset(std::filesystem::path path) {
    int img_width, img_height, img_nr_channels;
    // path.c_str() results differ depends on the operating system.
    unsigned char *data = stbi_load(path.string().c_str(), &img_width, &img_height, &img_nr_channels, 0);
    if (!data) {
        DEBUG_STDOUT("LoadAsset() : Texture load failed\n");
        return nullptr;
    }
    DEBUG_STDOUT("LoadAsset() : Texture loaded : %d %d %d\n", img_width, img_height, img_nr_channels);

    GLenum format = GL_RGB;
    if (img_nr_channels == 3) {
        format = GL_RGB;
    } else if (img_nr_channels == 4) {
        format = GL_RGBA;
    } else {
        DEBUG_STDOUT("Unexpected image channels : %d\n", img_nr_channels);
    }
    DEBUG_STDOUT("LoadAsset() : unpack alignment setting.\n");
    glPixelStorei(GL_UNPACK_ALIGNMENT, img_nr_channels);

    DEBUG_STDOUT("LoadAsset() : Ready to create texture...\n");

    std::unique_ptr<Texture> texture = std::make_unique<Texture>(
        img_width, img_height, img_nr_channels, data, format, format
    );

    stbi_image_free(data);

    return std::move(texture);
}

template<>
std::unique_ptr<TextureAtlas> LoadAsset(std::filesystem::path path) {
    if (path.extension() != ".xml") {
        DEBUG_STDOUT("LoadAsset<TextureAtlas>() : file extension is not .xml.\n");
        return nullptr;
    }

    std::unique_ptr<pugi::xml_document> doc = LoadAsset<pugi::xml_document>(path);
    if (!doc) {
        DEBUG_STDOUT("LoadAsset<TextureAtlas>() : Failed to load %s.\n", path.c_str());
        return nullptr;
    }
    if (std::string(doc->child("Asset").child("Type").child_value()) != "texture_atlas") {
        DEBUG_STDOUT("LoadAsset<TextureAtlas>() : Asset type is not texture_atlas.\n");
        return nullptr;
    }

    std::vector<SubTexture> bounds;
    for (auto sub_texture : doc->child("Asset").child("SubTextures").children("SubTexture")) {
        auto name = sub_texture.child("Name").child_value();
        auto pos = sub_texture.child("Position").child_value();
        auto size = sub_texture.child("Size").child_value();

        int pos_x, pos_y;
        std::stringstream(pos) >> pos_x >> pos_y;

        int size_width, size_height;
        std::stringstream(size) >> size_width >> size_height;

        DEBUG_STDOUT("\tTexture in atlas : %s (%d, %d) (%d, %d)\n",
            name, pos_x, pos_y, size_width, size_height);

        auto bound = SubTexture(
            std::string(name),
            glm::ivec2(pos_x, pos_y),
            glm::ivec2(size_width, size_height),
            nullptr
        );
        bounds.push_back(bound);
    }

    auto img_filepath = path.parent_path() / doc->child("Asset").child("Path").child_value();
    if (!std::filesystem::exists(img_filepath)) {
        DEBUG_STDOUT("LoadAsset<TextureAtlas>() : %s does not exist.\n", img_filepath.c_str());
        return nullptr;
    }

    std::unique_ptr<Texture> texture = LoadAsset<Texture>(img_filepath);
    std::unique_ptr<TextureAtlas> texture_atlas = std::make_unique<TextureAtlas>(
        std::move(texture), bounds
    );

    return std::move(texture_atlas);
}

template<>
std::unique_ptr<Shader> LoadAsset(std::filesystem::path path) {
    std::unique_ptr<std::string> shader_code = LoadAsset<std::string>(path);

    return std::move(std::make_unique<Shader>(GL_VERTEX_SHADER, shader_code->c_str()));
}

template<>
std::unique_ptr<ShaderProgram> LoadAsset(std::filesystem::path path) {
    std::unique_ptr<std::string> vertex_shader_code = LoadAsset<std::string>(path); // TODO:
    std::unique_ptr<std::string> fragment_shader_code = LoadAsset<std::string>(path);

    return std::move(std::make_unique<ShaderProgram>(
        vertex_shader_code->c_str(),
        fragment_shader_code->c_str()
    ));
}
