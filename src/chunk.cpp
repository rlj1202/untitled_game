#include "chunk.h"

#include "graphics/mesh_builder.h"

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

BlockType::BlockType(const std::string name, ITexture* texture)
    : name(name), texture(texture) {

}

Block::Block() : Block(nullptr) {

}

Block::Block(BlockType* type) : type(type) {

}

BlockType* Block::GetType() {
    return type;
}

Floor::Floor() {

}

Floor::Floor(FloorType* type) : type(type) {

}

Chunk::Chunk() {

}

Block& Chunk::GetBlock(const BlockCoord coord) {
    return blocks[coord.y][coord.x];
}

void Chunk::SetBlock(const BlockCoord coord, Block block) {
    blocks[coord.y][coord.x] = block;
}

void Chunk::Bake() {
    std::vector<Vertex> vertices = {
        Vertex(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)),
        Vertex(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
        Vertex(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
    };
    std::vector<unsigned int> indices = {
        0, 1, 2,
        0, 2, 3,
    };
    const MeshProfile meshprofile_quad(vertices, indices, nullptr);

    model = std::make_unique<Model>();

    for (int y = 0; y < CHUNK_SIZE; y++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            Block& block = blocks[y][x];

            model->Add(meshprofile_quad.Clone().Translate(glm::vec3(x, y, 0)).SetTexture(block.GetType()->texture));
        }
    }

    model->Bake();
}

Model& Chunk::GetModel() {
    return *model;
}

World::World() {

}

Chunk& World::GetChunk(const ChunkCoord coord) {
    return *chunks[coord];
}

void World::SetChunk(ChunkCoord coord, std::unique_ptr<Chunk> chunk) {
    chunks[coord] = std::move(chunk);
}