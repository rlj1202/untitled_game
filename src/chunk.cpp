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

BlockType::BlockType(const std::string name) : name(name) {

}

Block::Block() : Block(nullptr) {

}

Block::Block(const BlockType* type) : type(type) {

}

Floor::Floor() {

}

Floor::Floor(const FloorType* type) : type(type) {

}

Chunk::Chunk() {

}

void ChunkMakeMesh() {
    std::vector<Vertex> vertices = {
        Vertex(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)),
        Vertex(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
        Vertex(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
    };
    std::vector<unsigned int> indices = {
        0, 1, 2,
        0, 2, 3,
    };
    MeshProfile meshprofile_quad(vertices, indices, "");

    std::srand(0);

    MeshProfile meshprofile_tilemap;
    for (int x = -8; x < 8; x++) {
        for (int y = -8; y < 8; y++) {
            int tex_x = std::rand() % 3;
            int tex_y = std::rand() % 3;

            meshprofile_tilemap.Append(meshprofile_quad
                // .rotate(glm::radians(45.0f), glm::vec3(0, 0, 1))
                .TexScale(glm::vec2(1/16.0f, 1/16.0f))
                .TexTranslate(glm::vec2(tex_x/16.0f, tex_y/16.0f))
                .Translate(glm::vec3(x, y, 0)));
        }
    }
    
    // mesh = std::make_unique<Mesh>(BuildMesh(meshprofile_tilemap));
}

World::World() {

}

Chunk& World::GetChunk(const ChunkCoord coord) {
    return *chunks[coord];
}

void World::SetChunk(ChunkCoord coord, std::unique_ptr<Chunk> chunk) {
    chunks[coord] = std::move(chunk);
}