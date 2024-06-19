#include "data/mesh/WireCube.h"

#include "system/editor/Gizmo.h"

#pragma region Public Methods

WireCube::WireCube() : Mesh()
{
    Vertices = {
        // Front face
        Vertex {glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(0.f, 0.f, 1.f), glm::vec2(0.f, 0.f)},
        Vertex {glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(0.f, 0.f, 1.f), glm::vec2(1.f, 0.f)},
        Vertex {glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.f, 0.f, 1.f), glm::vec2(1.f, 1.f)},
        Vertex {glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(0.f, 0.f, 1.f), glm::vec2(0.f, 1.f)},

        // Back face
        Vertex {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.f, 0.f, -1.f), glm::vec2(0.f, 0.f)},
        Vertex {glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.f, 0.f, -1.f), glm::vec2(1.f, 0.f)},
        Vertex {glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(0.f, 0.f, -1.f), glm::vec2(1.f, 1.f)},
        Vertex {glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(0.f, 0.f, -1.f), glm::vec2(0.f, 1.f)}
    };

    Indices =
    {
        0, 1, 1, 2, 2, 3, 3, 0,
        4, 5, 5, 6, 6, 7, 7, 4,
        0, 4, 1, 5, 2, 6, 3, 7
    };

    setupMesh();
}

void WireCube::Draw(Shader* shader)
{
	glBindVertexArray(VAO);
    glLineWidth(Gizmo::GIZMO_WIDTH); // maybe set up in a constant file
	glDrawElements(GL_LINES, static_cast<GLsizei>(Indices.size()), GL_UNSIGNED_INT, 0);
	glLineWidth(1); // reset to default
    glBindVertexArray(0);
}

#pragma endregion