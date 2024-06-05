#pragma once

// glm
#include <maths/glm/glm.hpp>

#include "Vertex.h"

struct Triangle
{
    Vertex A;
    Vertex B;
    Vertex C;

    glm::vec3 NormalA;
    glm::vec3 NormalB;
    glm::vec3 NormalC;
};
