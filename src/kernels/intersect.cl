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
