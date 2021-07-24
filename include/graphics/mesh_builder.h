#ifndef H_MESH_BUILDER
#define H_MESH_BUILDER

#include <vector>

#ifdef EMSCRIPTEN
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

#include "graphics/mesh.h"

Mesh BuildMesh(const MeshProfile &profile);
Mesh BuildMesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices);

#endif