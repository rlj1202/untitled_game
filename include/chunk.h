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
    BlockType(std::string name);

    std::string name;
};

class Block {
public:
    Block();

private:
    BlockType *type;
};

class Chunk {
public:
    Chunk();

    Block& At(const BlockCoord coord);
    void Set(const BlockCoord coord, Block block);

private:
    Block blocks[CHUNK_SIZE][CHUNK_SIZE];

    // Mesh mesh;
};

class World {
public:
    World();

    Block& BlockAt(const WorldCoord coord);
    void SetBlock(const WorldCoord coord, Block block);

    Chunk& ChunkAt(const ChunkCoord coord);
    void SetChunk(const ChunkCoord coord, std::unique_ptr<Chunk> chunk);

private:
    std::map<ChunkCoord, std::unique_ptr<Chunk>> chunks;
};

#endif