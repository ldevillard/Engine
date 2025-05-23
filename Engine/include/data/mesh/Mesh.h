#pragma once

#include <vector>

// data
#include "data/Vertex.h"
#include "data/Texture.h"
#include "data/Triangle.h"

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
    Mesh& operator=(const Mesh& copy);
    ~Mesh();
    
    virtual void Draw(Shader* shader) const;
    int GetNumberOfTriangles() const;
    std::vector<Triangle> GetTriangles() const;

	// getters
	unsigned int GetVAO() const { return VAO; }
	unsigned int GetVBO() const { return VBO; }
	unsigned int GetEBO() const { return EBO; }

protected:
    //  render data
    unsigned int VAO, VBO, EBO;

    void setupMesh();
};
