#pragma once

// data
#include "data/Mesh.h"
#include "component/Component.h"

// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model : public Component
{

public:

    Model(std::string path, Shader* sh);

    int GetNumberOfTriangles() const;

    void Compute() override;

private:
    void draw();

    // model data
    std::vector<Mesh> meshes;
    std::vector<Texture> textures;
    std::string directory;

    Shader* shader = nullptr;

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};