#pragma once

#include <vector>

// data
#include "data/Vertex.h"
#include "data/Texture.h"
#include "data/mesh/MeshData.h"

#include "render/Shader.h"

class Mesh
{
public:
    // mesh data
    std::vector<Vertex>       Vertices = {};
    std::vector<unsigned int> Indices = {};
    std::vector<Texture>      Textures = {};

    Mesh();
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    Mesh(const Mesh& copy);
    
    virtual void Draw(Shader* shader);
    int GetNumberOfTriangles() const;

protected:
    //  render data
    unsigned int VAO, VBO, EBO;

    void setupMesh();
};
