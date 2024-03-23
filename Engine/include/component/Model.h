#pragma once

// data
#include "data/Mesh.h"
#include "component/Component.h"
#include "data/Material.h"

// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model : public Component
{

public:

    Model(std::string path, Material mat = Material::None);

    int GetNumberOfTriangles() const;

    void Compute() override;

private:
    void draw();

    // Model data
    std::vector<Mesh> meshes;
    std::vector<Texture> texturesLoaded;
    std::string directory;

    Material material = Material::None;

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};