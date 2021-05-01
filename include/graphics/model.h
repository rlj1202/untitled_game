#ifndef H_MODEL
#define H_MODEL

#include <vector>
#include <unordered_map>

#include "graphics/mesh.h"

class IModel {
public:
    virtual ~IModel() {}

    /**
     * @brief Draw meshes
     */
    virtual void Draw() = 0;
};

/**
 * @brief 
 * @details
 * 
 */
class Model : public IModel {
public:
    Model();

    /**
     * @brief Add mesh profile.
     */
    void Add(MeshProfile&& profile);

    /**
     * @brief Make meshes
     * @details
     * This function does not bake mesh profile into mesh just one by one.
     * It will gather meshes which have same textures into batches and bake that batches one by one.
     * This will enhence the performance by reducing state changes of OpenGL.
     */
    void Bake();

    void Draw() override;

    void Clear();

private:
    std::unordered_map<unsigned int, MeshProfile> mesh_profiles;

    std::vector<std::unique_ptr<IMesh>> meshes;
};

#endif