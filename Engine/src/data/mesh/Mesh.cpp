#include "data/mesh/Mesh.h"

#pragma region Public Methods

Mesh::Mesh() : VAO(0), VBO(0), EBO(0)
{
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
	this->Vertices = vertices;
	this->Indices = indices;
	this->Textures = textures;

	setupMesh();
}

Mesh::Mesh(const Mesh& copy)
{
	this->Vertices = copy.Vertices;
	this->Indices = copy.Indices;
	this->Textures = copy.Textures;

	setupMesh();
}

Mesh::~Mesh()
{
	// clear all previously allocated resources
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Mesh::Draw(Shader* shader) const
{
    // bind appropriate textures
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;
    for (unsigned int i = 0; i < Textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE1 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = Textures[i].Name;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++); // transfer unsigned int to string
        else if (name == "texture_normal")
            number = std::to_string(normalNr++); // transfer unsigned int to string
        else if (name == "texture_height")
            number = std::to_string(heightNr++); // transfer unsigned int to string

        // now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(shader->ID, (name + number).c_str()), i + 1);
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, Textures[i].ID);
    }

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(Indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

int Mesh::GetNumberOfTriangles() const
{
   return (int)Indices.size() / 3;
}

std::vector<Triangle> Mesh::GetTriangles() const
{
    std::vector<Triangle> triangles;

    triangles.reserve(GetNumberOfTriangles());
    for (int i = 0; i < Indices.size(); i += 3)
    {
        Vertex v1 = Vertices[Indices[i]];
        Vertex v2 = Vertices[Indices[i + 1]];
        Vertex v3 = Vertices[Indices[i + 2]];

        glm::vec3 min = glm::min(glm::min(v1.Position, v2.Position), v3.Position);
        glm::vec3 max = glm::max(glm::max(v1.Position, v2.Position), v3.Position);
        glm::vec3 center = (v1.Position + v2.Position + v3.Position) / 3.0f;

        triangles.push_back({ v1, v2, v3, min, max, center});
    }

    return triangles;
}

#pragma endregion

#pragma region Private Methods

void Mesh::setupMesh()
{
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), &Indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, UV));

    glBindVertexArray(0);
}

#pragma endregion
