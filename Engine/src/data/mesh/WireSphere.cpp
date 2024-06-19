#include "data/mesh/WireSphere.h"

#include <maths/glm/geometric.hpp>
#include <maths/glm/glm.hpp>
#include <maths/glm/gtc/matrix_transform.hpp>
#include <maths/glm/gtc/type_ptr.hpp>

#include <array>

#include "system/editor/Gizmo.h"

#pragma region Public Methods

WireSphere::WireSphere() : Mesh()
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float radius = 1.0f;
    int numVerticesPerCircle = 100;

    // Generate circles along X, Y, and Z axes
    generateCircle(vertices, indices, radius, numVerticesPerCircle, glm::vec3(1.0f, 0.0f, 0.0f)); // X axis
    generateCircle(vertices, indices, radius, numVerticesPerCircle, glm::vec3(0.0f, 1.0f, 0.0f)); // Y axis
    generateCircle(vertices, indices, radius, numVerticesPerCircle, glm::vec3(0.0f, 0.0f, 1.0f)); // Z axis

    Vertices = vertices;
    Indices = indices; 

    setupMesh();
}

void WireSphere::Draw(Shader* shader)
{
    glBindVertexArray(VAO);
    glLineWidth(Gizmo::GIZMO_WIDTH); // maybe set up in a constant file
    glDrawElements(GL_LINES, (GLsizei)Indices.size(), GL_UNSIGNED_INT, 0);
    glLineWidth(1); // reset to default
    glBindVertexArray(0);
}

#pragma endregion

#pragma region Private Methods

void WireSphere::generateCircle(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, float radius, int numVertices, const glm::vec3& axis)
{
    glm::vec3 xAxis(1.0f, 0.0f, 0.0f);
    glm::vec3 yAxis(0.0f, 1.0f, 0.0f);
    glm::vec3 zAxis(0.0f, 0.0f, 1.0f);

    // determine vectors perpendicular to the given axis
    glm::vec3 tangentVec, bitangentVec;
    if (axis == xAxis || axis == -xAxis) 
    {
        tangentVec = yAxis;
        bitangentVec = zAxis;
    }
    else if (axis == yAxis || axis == -yAxis) 
    {
        tangentVec = xAxis;
        bitangentVec = zAxis;
    }
    else if (axis == zAxis || axis == -zAxis) 
    {
        tangentVec = xAxis;
        bitangentVec = yAxis;
    }
    else 
    {
        std::cerr << "Invalid axis for circle generation" << std::endl;
    }

    // generate circle vertices
    for (int i = 0; i < numVertices; ++i) 
    {
        float angle = (glm::two_pi<float>() / static_cast<float>(numVertices)) * i;
        glm::vec3 position = radius * (glm::cos(angle) * tangentVec + glm::sin(angle) * bitangentVec);

        Vertex vertex;
        vertex.Position = position;
        vertex.Normal = glm::normalize(position); // normal = position for a sphere
        vertex.UV = glm::vec2(0.0f); // unused

        vertices.push_back(vertex);
    }

    // generate circle indices
    unsigned int startIndex = static_cast<unsigned int>(vertices.size()) - numVertices;
    for (unsigned int i = startIndex; i < startIndex + numVertices - 1; ++i)
    {
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    // connect the last vertex to the first vertex
    indices.push_back(startIndex + numVertices - 1);
    indices.push_back(startIndex);
}

#pragma endregion