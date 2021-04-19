#ifndef H_CHUNK
#define H_CHUNK

#include <map>
#include <memory>

#include "graphics/mesh.h"

const int CHUNK_SIZE = 16;

class ChunkCoord {
public:
    ChunkCoord(int x, int y);

    bool operator<(const ChunkCoord &o) const;

    int x, y;
};

class BlockCoord {
public:
    BlockCoord(int x, int y);
    
    int x, y;
};

class WorldCoord {
public:
    WorldCoord(int x, int y);

    operator ChunkCoord();
    operator BlockCoord();

    int x, y;
};

WorldCoord operator+(const ChunkCoord chunk_coord, const BlockCoord block_coord);
WorldCoord operator+(const BlockCoord block_coord, const ChunkCoord chunk_coord);

class BlockType {
public:
    BlockType(const std::string name);

    const std::string name;
};

class FloorType {
public:
    FloorType(const std::string name);

    /**
     * Name to idendify type.
     */
    const std::string name;

    /**
     * Moving speed of entity on this type of floor.
     */
    const float speed;
};

class Catalog {
public:
    Catalog();


};

class Block {
public:
    Block();
    Block(const BlockType* type);

private:
    const BlockType* type;
};

class Floor {
public:
    Floor();
    Floor(const FloorType* type);

private:
    const FloorType* type;
};

class Chunk {
public:
    Chunk();

    Floor& GetFloor(const BlockCoord coord);
    void SetFloor(const BlockCoord coord, Floor floor);

    Block& GetBlock(const BlockCoord coord);
    void SetBlock(const BlockCoord coord, Block block);

    void Bake();

private:
    Floor floors[CHUNK_SIZE][CHUNK_SIZE];
    Block blocks[CHUNK_SIZE][CHUNK_SIZE];

    std::unique_ptr<Mesh> mesh;
};

class World {
public:
    World();

    Floor& GetFloor(const WorldCoord coord);
    void SetFloor(const WorldCoord coord, Floor floor);

    Block& GetBlock(const WorldCoord coord);
    void SetBlock(const WorldCoord coord, Block block);

    Chunk& GetChunk(const ChunkCoord coord);
    void SetChunk(const ChunkCoord coord, std::unique_ptr<Chunk> chunk);

private:
    std::map<ChunkCoord, std::unique_ptr<Chunk>> chunks;
};

#endif