#include "intersect.h"

float intersectPlane(struct Ray ray, struct Plane plane)
{
    float d = dot(plane.normal, ray.direction);
    if (d != 0.0f)
        return -(dot(plane.normal, ray.origin) + plane.offset) / d;
    else
        return INFINITY;
}

float intersectSphere(struct Ray ray, struct Sphere sphere)
{
    float3 oc = ray.origin - sphere.center;
    float b = dot(ray.direction, oc);
    float c = dot(oc, oc) - pow(sphere.radius, 2.0f);
    float d = pow(b, 2.0f) - c;

    if (d < 0.0f)
        return INFINITY;
    else
        return -b - sqrt(d);
}

float intersectTriangle(struct Ray ray, struct Triangle triangle)
{
    return INFINITY;
}
