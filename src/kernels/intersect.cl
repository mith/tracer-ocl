///////

#include "intersect.h"

float3 intersectTriangle(struct Ray ray, struct Triangle triangle)
{
    // Moller-Trumbore
    float3 e1 = triangle.b.position - triangle.a.position;
    float3 e2 = triangle.c.position - triangle.a.position;
    float3 P = cross(ray.direction, e2);
    float det = dot(e1, P);
    if (det > -FLT_EPSILON && det < FLT_EPSILON) return (float3)(INFINITY, INFINITY, INFINITY);

    float inv_det = 1.0f / det;
    float3 T = ray.origin - triangle.a.position;
    float u = dot(T, P) * inv_det;
    if (u < 0.0f || u > 1.0f) return (float3)(INFINITY, INFINITY, INFINITY);

    float3 Q = cross(T, e1);
    float v = dot(ray.direction, Q) * inv_det;
    if (v < 0.0f || u + v > 1.0f) return (float3)(INFINITY, INFINITY, INFINITY);

    float t = dot(e2, Q) * inv_det;
    if (t > FLT_EPSILON) {
        return (float3)(u, v, t);
    } else {
        return (float3)(INFINITY, INFINITY, INFINITY);
    }
}

float intersectAABB(struct Ray ray, struct AABB aabb)
{
    float tx1 = (aabb.min.x - ray.origin.x) * ray.direction_inverse.x;
    float tx2 = (aabb.max.x - ray.origin.x) * ray.direction_inverse.x;

    float tmin = min(tx1, tx2);
    float tmax = max(tx1, tx2);

    float ty1 = (aabb.min.y - ray.origin.y) * ray.direction_inverse.y;
    float ty2 = (aabb.max.y - ray.origin.y) * ray.direction_inverse.y;

    tmin = max(tmin, min(ty1, ty2));
    tmax = min(tmax, max(ty1, ty2));

    float tz1 = (aabb.min.z - ray.origin.z) * ray.direction_inverse.z;
    float tz2 = (aabb.max.z - ray.origin.z) * ray.direction_inverse.z;

    tmin = max(tmin, min(tz1, tz2));
    tmax = min(tmax, max(tz1, tz2));

    return tmax >= tmin;
}

