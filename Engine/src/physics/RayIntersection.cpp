#include "physics/RayIntersection.h"

#include "data/BoundingBox.h"
#include "data/physics/HitInfo.h"
#include "data/physics/Ray.h"
#include "data/Triangle.h"
#include "maths/Math.h"

bool RayAABoxIntersection(const Ray& ray, const BoundingBox& box, HitInfo& outHitInfo)
{
    glm::vec3 tMin = (box.Min - ray.origin) / ray.direction;
    glm::vec3 tMax = (box.Max - ray.origin) / ray.direction;

    // find the nearest and farthest intersections
    glm::vec3 tNear = glm::min(tMin, tMax);
    glm::vec3 tFar = glm::max(tMin, tMax);

    // find the maximum of the nearest intersections
    float tNearMax = glm::max(glm::max(tNear.x, tNear.y), tNear.z);
    // find the minimum of the farthest intersections
    float tFarMin = glm::min(glm::min(tFar.x, tFar.y), tFar.z);

    // check if the ray intersects the bounding box
    if (tNearMax <= tFarMin && outHitInfo.distance > tNearMax)
    {
        // ray intersects the bounding box
        // calculate the intersection point
        glm::vec3 intersectionPoint = ray.origin + ray.direction * tNearMax;
        outHitInfo.hitPoint = intersectionPoint;
        outHitInfo.distance = tNearMax;
        outHitInfo.hit = true;

        return true;
    }

    // ray does not intersect the bounding box
    return false;
}

bool RayTriangleIntersection(const Ray& ray, const Triangle& triangle, HitInfo& outHitInfo)
{
    glm::vec3 AB = triangle.B.Position - triangle.A.Position;
    glm::vec3 AC = triangle.C.Position - triangle.A.Position;
    glm::vec3 rayOriginAOffset = ray.origin - triangle.A.Position;

    glm::vec3  n = cross(AB, AC);
    glm::vec3  q = cross(rayOriginAOffset, ray.direction);

    float d = 1.0f / dot(ray.direction, n);
    float u = d * dot(-q, AC);
    float v = d * dot(q, AB);
    float t = d * dot(-n, rayOriginAOffset);

    if (u < 0.0f || v < 0.0f || (u + v) > 1.0f)
        t = -1.0f;

    if (t > 0)
    {
        outHitInfo.hit = true;
        outHitInfo.distance = t;
        outHitInfo.hitPoint = ray.origin + t * ray.direction;
        return true;
    }

    return false;
}