#pragma once

#include <map>

// data
#include "data/mesh/Mesh.h"
#include "component/Component.h"
#include "data/Material.h"
#include "data/mesh/MeshData.h"

// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model : public Component
{

public:
    Model(PrimitiveType type, Material mat = Material::Default);
    Model(std::string path, Material mat = Material::Default);
    Model(const Mesh& mesh, Material mat = Material::Default);

    int GetNumberOfTriangles() const;
    const Material& GetMaterial() const;

    void Compute() override;

    void SetMaterialFromName(std::string name);

    // static models
    static void LoadPrimitives();
    static std::map<PrimitiveType, std::unique_ptr<Model>> PrimitivesModels;

private:
    void draw();

    // model data
    std::vector<Mesh> meshes;
    std::vector<Texture> texturesLoaded;
    std::string directory;

    Material material = Material::Default;

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};