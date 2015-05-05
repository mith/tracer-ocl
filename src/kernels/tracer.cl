/////

#include "tracer.h"
#include "intersect.h"
#include "shader.h"

struct Ray createCameraRay(int2 coord)
{
    const float viewport_width = 2.0f;
    const float viewport_height = 1.0f;

    const float x_ratio = viewport_width / (float)get_global_size(0);
    const float y_ratio = viewport_height / (float)get_global_size(1);

    struct Ray ray;
    ray.direction = normalize((float3)(coord.x * x_ratio - 1.0f,
                                       coord.y * y_ratio - 0.5f,
                                       -0.7f));
    ray.origin = (float3)(0.0f, 0.0f, 0.0f);
    return ray;
}

float3 rayPoint(struct Ray ray, float t)
{
    return ray.origin + ray.direction * t;
}

float lengthSquared(float3 a)
{
    return a.x * a.x + a.y * a.y + a.z * a.z;
}

float3 reflect(float3 i, float3 n)
{
    return i - 2.0f * n * dot(n, i);
}

struct RayHit traceRayAgainstPlanes(struct Ray ray,
                                    global const struct Plane* planes,
                                    int numPlanes)
{
    struct RayHit nearestHit;
    nearestHit.dist = (float)(INFINITY);
    nearestHit.material = -1;

    for (int p = 0; p < numPlanes; p++) {
        struct Plane plane = planes[p];

        float t = intersectPlane(ray, plane);

        if (t < 0.0001f) {
            continue;
        }

        float3 loc = rayPoint(ray, t);
        float dist = distance(ray.origin, loc);

        if (nearestHit.dist > dist) {
            nearestHit.dist = dist;
            nearestHit.location = loc;
            nearestHit.normal = plane.normal;
            nearestHit.material = plane.material;
            nearestHit.object = &planes[p];
        }
    }
    return nearestHit;
}

struct RayHit traceRayAgainstSpheres(struct Ray ray,
                                     global const struct Sphere* spheres,
                                     int numSpheres)
{
    struct RayHit nearestHit;
    nearestHit.dist = (float)(INFINITY);
    nearestHit.material = -1;

    for (int s = 0; s < numSpheres; s++) {
        struct Sphere sphere = spheres[s];
        float t = intersectSphere(ray, sphere);
        float3 loc = rayPoint(ray, t);
        float3 normal = normalize(loc - sphere.center);

        float dist = distance(loc, ray.origin);

        if (nearestHit.dist > dist) {
            nearestHit.dist = dist;
            nearestHit.location = loc;
            nearestHit.normal = normal;
            nearestHit.material = sphere.material;
            nearestHit.object = &spheres[s];
        }
    }

    return nearestHit;
}

struct RayHit traceRayAgainstTriangles(struct Ray ray,
                                       global const struct Triangle* triangles,
                                       int numTriangles)
{
    struct RayHit nearestHit;
    nearestHit.dist = (float)(INFINITY);
    nearestHit.material = -1;

    for (int i = 0; i < numTriangles; i++) {
        struct Triangle triangle = triangles[i];
        float t = intersectTriangle(ray, triangle);
        float3 loc = rayPoint(ray, t);

        float3 v = triangle.b - triangle.a;
        float3 w = triangle.c - triangle.a;
        float3 normal = normalize(cross(v, w));

        float dist = distance(loc, ray.origin);

        if (nearestHit.dist > dist) {
            nearestHit.dist = dist;
            nearestHit.location = loc;
            nearestHit.normal = normal;
            nearestHit.material = triangle.material;
            nearestHit.object = &triangles[i];
        }
    }

    return nearestHit;
}

void kernel tracer(write_only image2d_t img,
                   global const struct Light* lights,
                   int numLights,
                   global const struct Plane* planes,
                   int numPlanes,
                   global const struct Sphere* spheres,
                   int numSpheres,
                   global const struct Triangle* triangles,
                   int numTriangles,
                   global const struct Material* materials)
{
    const int2 coord = (int2)(get_global_id(0), get_global_id(1));
    struct Ray ray = createCameraRay(coord);
    struct RayHit planeHit = traceRayAgainstPlanes(ray, planes, numPlanes);
    struct RayHit sphereHit = traceRayAgainstSpheres(ray, spheres, numSpheres);
    struct RayHit triangleHit = traceRayAgainstTriangles(ray, triangles, numTriangles);
    float3 color = (float3)(0.0f, 0.0f, 0.0f);
    if (planeHit.dist < sphereHit.dist && planeHit.dist < triangleHit.dist) {
        color = gatherLight(ray, planeHit, spheres, numSpheres,
                            triangles, numTriangles, lights, numLights, materials);
    } else if (triangleHit.dist < sphereHit.dist) {
        color = gatherLight(ray, triangleHit, spheres, numSpheres,
                            triangles, numTriangles, lights, numLights, materials);
    } else {
        color = gatherLight(ray, sphereHit, spheres, numSpheres,
                            triangles, numTriangles, lights, numLights, materials);
    }
    write_imagef(img, coord, (float4)(color, 10.f));
}
