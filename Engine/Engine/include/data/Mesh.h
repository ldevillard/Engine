#pragma once

#include <vector>

// data
#include "data/Vertex.h"
#include "data/Texture.h"

#include "Shader.h"

class Mesh
{
public:
    // mesh data
    std::vector<Vertex>       Vertices;
    std::vector<unsigned int> Indices;
    std::vector<Texture>      Textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void Draw(Shader& shader);

private:
    //  render data
    unsigned int VAO, VBO, EBO;

    void setupMesh();
};
