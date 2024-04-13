#pragma once

enum PrimitiveType
{
    CubePrimitive,
    SpherePrimitive,
	PlanePrimitive,
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
        // Positions       // Texture coordinates
        -1.0f,  1.0f,      0.0f, 1.0f, // Top-left
        -1.0f, -1.0f,      0.0f, 0.0f, // Top-right
         1.0f, -1.0f,      1.0f, 0.0f, // Bottom-left
        -1.0f,  1.0f,      0.0f, 1.0f, // Bottom-right
         1.0f, -1.0f,      1.0f, 0.0f, // Bottom-right
         1.0f,  1.0f,      1.0f, 1.0f  // Top-right
    };
};