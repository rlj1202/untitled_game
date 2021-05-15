/**
 * @file
 * 
 * @brief Manages asset files such as texture, audio, block type definition file, etc.
 * @details
 * 
 */
#ifndef H_ASSET
#define H_ASSET

#include <memory>
#include <string>
#include <filesystem>
#include <unordered_map>
#include <typeindex>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

/**
 * @brief Special file name for an asset.
 * @details
 * 
 */
const std::wstring ASSET_FILE_NAME = L"asset.xml";

class IAssetList {
public:
    virtual ~IAssetList() {}
};

template<typename T>
class AssetList : public IAssetList {
public:
    AssetList();

    void SetAsset(const std::wstring asset_path, std::unique_ptr<T> asset);

    T* GetAsset(const std::wstring asset_path);

    std::vector<std::wstring> GetLoadedAssetList() const;

private:
    std::unordered_map<std::wstring, std::unique_ptr<T>> assets;
};

/**
 * @brief Manages all assets.
 * @details
 * In the following file system:
 * 
 * /res
 *   /textures
 *     /some_texture.png
 *     /some_texture2.jpg
 *     /atlas
 *       /asset.xml
 *       /texture.png
 * 
 * To refer some_texture.png file, refer "textures/some_texture".
 * To refer atlas.xml, refer "textures/atlas".
 * 
 * Here, asset.xml is a special file that indicates its parent folder is one asset as a whole.
 * 
 * How the assets are loaded is depend on an asset loader.
 */
class AssetManager {
public:
    AssetManager(std::filesystem::path root_dir);

    /**
     * @brief Resolve asset path to full path in real filesystem.
     * @details
     * 
     */
    std::filesystem::path ResolveAssetPath(const std::wstring path) const;

    /**
     * @brief Set asset
     * @details
     * 
     * 
     */
    template<typename T>
    void SetAsset(const std::wstring asset_path, std::unique_ptr<T> asset);

    /**
     * @brief Get asset
     * @details
     * 
     */
    template<typename T>
    T* GetAsset(const std::wstring asset_path);

    template<typename T>
    AssetList<T>* GetAssetList();

private:
    std::filesystem::path root_dir;

    std::unordered_map<std::type_index, std::unique_ptr<IAssetList>> asset_lists;
};

template<typename T>
std::unique_ptr<T> LoadAsset(std::filesystem::path path);

#include "asset.tcc"

#endif