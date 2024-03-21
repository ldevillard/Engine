#pragma once

// data
#include "data/Mesh.h"
#include "component/Component.h"

// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model : Component
{

public:

    Model(std::string path);

    void Draw(Shader& shader);
    int GetNumberOfTriangles() const;

private:

    // model data
    std::vector<Mesh> Meshes;
    std::vector<Texture> Textures;
    std::string Directory;

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};