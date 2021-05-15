#include "graphics/model.h"

#include <algorithm>

#include "graphics/mesh_builder.h"

#define DEBUG
#include "debug.h"

Model::Model() {

}

void Model::Add(MeshProfile& profile) {
    if (profile.texture == nullptr) return; // TODO:

    mesh_profiles[profile.texture->GetId()].Append(
        profile.Clone().TexMul(profile.texture->GetTextureTransformationMatrix())
    );
}

void Model::Bake() {
    // Destroy all existing meshes.
    meshes.clear();

    for (auto& [ texture, profile ] : mesh_profiles) {
        std::unique_ptr<IMesh> mesh = std::make_unique<Mesh>(BuildMesh(profile));
        
        meshes.push_back(std::move(mesh));
    }

    DEBUG_STDOUT(
        "Model::Bake() : %d mesh profiles, %d meshes.\n",
        (int) mesh_profiles.size(), (int) meshes.size()
    );
}

void Model::Draw() {
    for (auto& mesh : meshes) {
        mesh->Draw();
    }
}

void Model::Clear() {
    mesh_profiles.clear();
    meshes.clear();
}