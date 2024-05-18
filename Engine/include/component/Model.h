#pragma once

#include <map>

// data
#include "component/Component.h"
#include "data/mesh/Mesh.h"
#include "data/Material.h"
#include "data/mesh/MeshData.h"
#include "data/OBoundingBox.h"

// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class EditorCollider;

class Model : public Component
{

public:
    Model(PrimitiveType type, Material mat = Material::Default);
    Model(std::string path, Material mat = Material::Default);
    Model(const Mesh& mesh, Material mat = Material::Default);

    int GetNumberOfTriangles() const;
    Material& GetMaterial();
    const OBoundingBox& GetBoundingBox() const;

    void Compute() override;
    Component* Clone() override;
    void ComputeOutline(Shader* outlineShader);

    void SetMaterialFromName(std::string name);
    void SetEditorCollider(EditorCollider* cl) override;

    // static models
    static void LoadPrimitives();
    static std::map<PrimitiveType, std::unique_ptr<Model>> PrimitivesModels;

    PrimitiveType ModelType = PrimitiveType::None;

private:
    Model() = default;
    void draw();

    // model data
    std::vector<Mesh> meshes;
    std::vector<Texture> texturesLoaded;
    std::string directory;

    Material material = Material::Default;

    void processOBB();
    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};