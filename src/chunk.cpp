#include "chunk.h"

WorldCoord::WorldCoord(int x, int y) : x(x), y(y) {}

WorldCoord::operator ChunkCoord() {
    return ChunkCoord(x / CHUNK_SIZE, y / CHUNK_SIZE);
}
WorldCoord::operator BlockCoord() {
    return BlockCoord(x % CHUNK_SIZE, y / CHUNK_SIZE);
}

ChunkCoord::ChunkCoord(int x, int y) : x(x), y(y) {}

bool ChunkCoord::operator<(const ChunkCoord &o) const {
    if (x == o.x) return y < o.y;
    return x < o.x;
}

BlockCoord::BlockCoord(int x, int y) : x(x), y(y) {}

WorldCoord operator+(const ChunkCoord chunk_coord, const BlockCoord block_coord) {
    return WorldCoord(
        chunk_coord.x * CHUNK_SIZE + block_coord.x,
        chunk_coord.y * CHUNK_SIZE + block_coord.y
        );
}
WorldCoord operator+(const BlockCoord block_coord, const ChunkCoord chunk_coord) {
    return chunk_coord + block_coord;
}

BlockType::BlockType(std::string name) : name(name) {

}

Block::Block() {

}

Chunk::Chunk() {

}

World::World() {

}

Chunk& World::ChunkAt(const ChunkCoord coord) {
    return *chunks[coord];
}

void World::SetChunk(ChunkCoord coord, std::unique_ptr<Chunk> chunk) {
    chunks[coord] = std::move(chunk);
}