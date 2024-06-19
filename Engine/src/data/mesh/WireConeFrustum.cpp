#include "data/mesh/WireConeFrustum.h"

#include "system/editor/Gizmo.h"

#pragma region Public Methods

WireConeFrustum::WireConeFrustum() : Mesh()
{
    generateConeFrustum(32, 1.f, 0.25f, 3.0f);
    setupMesh();
}

void WireConeFrustum::Draw(Shader* shader)
{
    glBindVertexArray(VAO);
    glLineWidth(Gizmo::GIZMO_WIDTH); // maybe set up in a constant file
    glDrawElements(GL_LINES, static_cast<GLsizei>(Indices.size()), GL_UNSIGNED_INT, 0);
    glLineWidth(1); // reset to default
    glBindVertexArray(0);
}

#pragma endregion

#pragma region Private Methods

void WireConeFrustum::generateConeFrustum(unsigned int edgeCount, float topRadius, float bottomRadius, float height)
{
    // base vertices
    float halfHeight = height / 2.0f;

    // Generate vertices for the bottom circle
    float angleIncrement = glm::radians(360.0f / edgeCount);
    for (unsigned int i = 0; i < edgeCount; ++i) 
    {
        float angle = i * angleIncrement;
        Vertices.push_back(Vertex(glm::vec3(cos(angle) * bottomRadius, -halfHeight, sin(angle) * bottomRadius), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(cos(angle) * 0.5f + 0.5f, sin(angle) * 0.5f + 0.5f)));
    }

    // Generate vertices for the top circle
    for (unsigned int i = 0; i < edgeCount; ++i) 
    {
        float angle = i * angleIncrement;
        Vertices.push_back(Vertex(glm::vec3(cos(angle) * topRadius, halfHeight, sin(angle) * topRadius), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(cos(angle) * 0.5f + 0.5f, sin(angle) * 0.5f + 0.5f)));
    }

    // Connect bottom circle vertices
    for (unsigned int i = 0; i < edgeCount - 1; ++i) 
    {
        Indices.push_back(i);
        Indices.push_back(i + 1);
    }
    Indices.push_back(edgeCount - 1);
    Indices.push_back(0);

    // Connect top circle vertices
    unsigned int topOffset = edgeCount;
    for (unsigned int i = topOffset; i < topOffset + edgeCount - 1; ++i) 
    {
        Indices.push_back(i);
        Indices.push_back(i + 1);
    }
    Indices.push_back(topOffset + edgeCount - 1);
    Indices.push_back(topOffset);

    // Connect top and bottom circles
    for (unsigned int i = 0; i < edgeCount; ++i) 
    {
        if (i % 8 != 0) // skip every 8th line to make it look better, work only with 32 edges to get 4 lines
            continue;
        Indices.push_back(i);
        Indices.push_back(i + topOffset);
    }
}

#pragma endregion