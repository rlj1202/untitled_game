#ifndef H_LUVOASI_MAP
#define H_LUVOASI_MAP

#include <memory>
#include <string>

#include "luvoasi/core/base.h"

namespace Luvoasi::Map {

const int CHUNK_SIZE = 16;

/**
 * @brief 
 * 
 */
class Material {
public:
    /// Unique string to distinguish material from others.
    std::string id;

    /// Human readable name.
    std::wstring name;

    /// Texture path
    std::wstring texture_path;


};

class MaterialCatalog {
public:

};

class Block {
public:
    Block();

private:
    Material* m_material;
};

class Chunk {
public:
    virtual ~Chunk() = default;
};

class World {
public:
    virtual ~World() = default;

    virtual const Block* GetBlockAt() const = 0;
    virtual Block* GetBlockAt() = 0;
    virtual void SetBlockAt(const Block& block) = 0;
};

/**
 * @brief chunk in client side.
 * 
 */
class LocalChunk : public Chunk {
public:
    LocalChunk();

private:
    Block* m_blocks[CHUNK_SIZE][CHUNK_SIZE];
};

/**
 * @brief world in client side.
 * 
 */
class LocalWorld : public World {
public:
    LocalWorld();

private:
};

/**
 * @brief world in remote side.
 * 
 */
class RemoteWorld : public World {
public:
    RemoteWorld();

private:

};

/**
 * @brief 
 * 
 */
class Map {
public:
    Map();

    static bool SaveTo(const std::string& path);
    static std::unique_ptr<Map> LoadFrom(const std::string& path);

private:
    std::unique_ptr<World> m_world;
    // std::vector<some class> entities;
};

}

#endif