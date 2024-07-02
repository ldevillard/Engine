#pragma once

// glm
#include <maths/glm/glm.hpp>

#include "Vertex.h"

struct Triangle
{
    Vertex A;
    Vertex B;
    Vertex C;
    
    glm::vec3 Min;
    glm::vec3 Max;
    glm::vec3 Center;
};
