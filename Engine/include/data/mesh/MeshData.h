#pragma once

#include <maths/glm/glm.hpp>

enum PrimitiveType
{
    None,

    CubePrimitive,
    SpherePrimitive,
    PlanePrimitive,
    CylinderPrimitive,
    TorusPrimitive
};

enum GizmoType
{
    WireCubeGizmo,
    WireSphereGizmo,
    WireConeGizmo,
    WireConeFrustumGizmo,
};

struct ScreenQuad
{
    unsigned int VAO = 0;
    unsigned int VBO = 0;

    float vertices[24] =
    {
        // positions       // texture coordinates
        -1.0f,  1.0f,      0.0f, 1.0f, // top-left
        -1.0f, -1.0f,      0.0f, 0.0f, // top-right
         1.0f, -1.0f,      1.0f, 0.0f, // bottom-left
        -1.0f,  1.0f,      0.0f, 1.0f, // bottom-right
         1.0f, -1.0f,      1.0f, 0.0f, // bottom-right
         1.0f,  1.0f,      1.0f, 1.0f  // top-right
    };
};

struct ScreenCube
{
    unsigned int VAO = 0;
    unsigned int VBO = 0;

    float vertices[108] = 
    {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
};

struct Sphere
{
    glm::vec3 Position;
    float Radius;
};