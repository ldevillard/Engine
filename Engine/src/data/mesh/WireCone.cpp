#include "data/mesh/WireCone.h"

#pragma region Public Methods

WireCone::WireCone() : Mesh()
{
    generateCone(25);

	setupMesh();
}

void WireCone::Draw(Shader* shader)
{
	glBindVertexArray(VAO);
	glLineWidth(3); // maybe set up in a constant file
	glDrawElements(GL_LINES, Indices.size(), GL_UNSIGNED_INT, 0);
	glLineWidth(1); // reset to default
	glBindVertexArray(0);
}

#pragma endregion

#pragma region Private Methods

void WireCone::generateCone(unsigned int edgeCount)
{
    // Base radius and height
    float radius = 0.1f;
    float height = 0.2f;

    // Base vertices
    Vertices.push_back(Vertex(glm::vec3(0.0f, -height / 2.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.5f, 0.5f))); // Center of the base
    float angleIncrement = glm::radians(360.0f / edgeCount);
    for (unsigned int i = 0; i < edgeCount; ++i) {
        float angle = i * angleIncrement;
        Vertices.push_back(Vertex(glm::vec3(cos(angle) * radius, -height / 2.0f, sin(angle) * radius), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(cos(angle) * 0.5f + 0.5f, sin(angle) * 0.5f + 0.5f)));
    }

    // Apex vertex
    Vertices.push_back(Vertex(glm::vec3(0.0f, height / 2.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.5f, 0.5f))); // Apex

    // Connect base vertices to the apex
    for (unsigned int i = 1; i <= edgeCount; ++i) {
        Indices.push_back(0);
        Indices.push_back(i);
    }
    Indices.push_back(0);
    Indices.push_back(1);

    // Connect edges of the base
    for (unsigned int i = 1; i < edgeCount; ++i) {
        Indices.push_back(i);
        Indices.push_back(i + 1);
    }
    Indices.push_back(edgeCount);
    Indices.push_back(1);

    // Connect edges from the apex to the base
    for (unsigned int i = 1; i <= edgeCount; ++i) {
        Indices.push_back(edgeCount + 1);
        Indices.push_back(i);
    }
}

#pragma endregion