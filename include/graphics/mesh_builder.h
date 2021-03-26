#ifndef H_MESH_BUILDER
#define H_MESH_BUILDER

#include <vector>

#include <GLES3/gl3.h>

#include "graphics/mesh.h"

Mesh BuildMesh(const MeshProfile &profile);
Mesh BuildMesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices);

#endif