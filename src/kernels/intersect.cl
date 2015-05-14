///////

#include "intersect.h"

float intersectPlane(struct Ray ray, struct Plane plane)
{
    float d = dot(plane.normal, ray.direction);
    if (d != 0.0f) {
        float NdotO = dot(plane.normal, ray.origin);
        float nrm = -(NdotO + plane.offset);
        return nrm / d;
    } else {
        return (float)INFINITY;
    }
}

float intersectSphere(struct Ray ray, struct Sphere sphere)
{
    float3 oc = ray.origin - sphere.center;
    float b = dot(ray.direction, oc);
    float c = dot(oc, oc) - pow(sphere.radius, 2.0f);
    float d = pow(b, 2.0f) - c;
    
    if (d < 0.0f)
        return (float)INFINITY;
    else
        return -b - sqrt(d);
}

float intersectTriangle(struct Ray ray, struct Triangle triangle)
{
    // Moller-Trumbore
    float3 e1 = triangle.b - triangle.a;
    float3 e2 = triangle.c - triangle.a;
    float3 P = cross(ray.direction, e2);
    float det = dot(e1, P);
    if (det > -FLT_EPSILON && det < FLT_EPSILON) return INFINITY;

    float inv_det = 1.0f / det;
    float3 T = ray.origin - triangle.a;
    float u = dot(T, P) * inv_det;
    if (u < 0.0f || u > 1.0f) return INFINITY;

    float3 Q = cross(T, e1);
    float v = dot(ray.direction, Q) * inv_det;
    if (v < 0.0f || u + v > 1.0f) return INFINITY;

    float t = dot(e2, Q) * inv_det;
    if (t > FLT_EPSILON) {
        return t;
    } else {
        return INFINITY;
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

