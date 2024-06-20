#pragma once

class BoundingBox;
struct HitInfo;
struct Ray;
struct Triangle;

bool RayAABoxIntersection(const Ray& ray, const BoundingBox& box, HitInfo& outHitInfo);
bool RayTriangleIntersection(const Ray& ray, const Triangle& triangle, HitInfo& outHitInfo);