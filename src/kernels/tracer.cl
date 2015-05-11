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
    ray.direction_inverse = 1.0f / ray.direction;
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

        if (nearestHit.dist > t && t > 0.0f) {
            float3 loc = rayPoint(ray, t);
            nearestHit.dist = t;
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

        if (nearestHit.dist > t && t > 0.0f) {
            float3 loc = rayPoint(ray, t);
            float3 normal = normalize(loc - sphere.center);
            nearestHit.dist = t;
            nearestHit.location = loc;
            nearestHit.normal = normal;
            nearestHit.material = sphere.material;
            nearestHit.object = &spheres[s];
        }
    }

    return nearestHit;
}

struct Triangle constructTriangle(global const struct Vertex* vertices,
                                  global const struct Indices* indices,
                                  int numTriangle,
                                  struct Mesh mesh)
{
    uint3 i = indices[numTriangle + mesh.base_triangle].vertex;
    struct Triangle triangle;
    triangle.a = vertices[i.x + mesh.base_vertex].position * mesh.scale + mesh.position;
    triangle.b = vertices[i.y + mesh.base_vertex].position * mesh.scale + mesh.position;
    triangle.c = vertices[i.z + mesh.base_vertex].position * mesh.scale + mesh.position;

    return triangle;
}

struct RayHit traceRayAgainstMesh(struct Ray ray,
                                  global const struct Vertex* vertices,
                                  global const struct Indices* indices,
                                  global const struct Mesh* meshes,
                                  int numMesh)
{
    struct RayHit nearestHit;
    nearestHit.dist = (float)(INFINITY);

    struct Mesh mesh = meshes[numMesh];
    for (int p = 0; p < mesh.num_triangles; p++) {
        struct Triangle triangle = constructTriangle(vertices, indices, p,
                                                     mesh);
        float t = intersectTriangle(ray, triangle);

        if (nearestHit.dist > t && t > 0.0f) {
            float3 loc = rayPoint(ray, t);
            float3 v = triangle.b - triangle.a;
            float3 w = triangle.c - triangle.a;
            float3 normal = normalize(cross(v, w));
            nearestHit.dist = t;
            nearestHit.location = loc;
            nearestHit.normal = -normal;
            nearestHit.material = mesh.material;
            nearestHit.object = &indices[p];
        }
    }
    return nearestHit;
}

struct RayHit traceRayAgainstBVH(struct Ray ray,
                                 global const struct BVHNode* bvh,
                                 int numBVHNodes,
                                 global const struct Vertex* vertices,
                                 global const struct Indices* indices,
                                 global const struct Mesh* meshes,
                                 int numMeshes)
{
    struct RayHit nearestHit;
    nearestHit.dist = (float)(INFINITY);
    for (int b = 0; b < numBVHNodes; b++) {
        struct BVHNode bvhnode = bvh[b];
        if (false) {//intersectAABB(ray, bvhnode.bounds)) {
            struct RayHit hit = traceRayAgainstMesh(ray, vertices, indices, meshes, bvhnode.mesh);
            if (hit.dist < nearestHit.dist) {
                nearestHit = hit;
            }
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
                   global const struct Vertex* vertices,
                   global const struct Indices* indices,
                   global const struct Mesh* meshes,
                   int numMeshes,
                   global const struct BVHNode* bvh,
                   int numBVHNodes,
                   global const struct Material* materials)
{
    const int2 coord = (int2)(get_global_id(0), get_global_id(1));
    struct Ray ray = createCameraRay(coord);
    struct RayHit planeHit = traceRayAgainstPlanes(ray, planes, numPlanes);
    struct RayHit sphereHit = traceRayAgainstSpheres(ray, spheres, numSpheres);
    struct RayHit meshHit = traceRayAgainstBVH(ray, bvh, numBVHNodes, vertices, 
                                               indices, meshes, numMeshes);

    float3 color = (float3)(0.0f, 0.0f, 0.0f);
    if (planeHit.dist < sphereHit.dist && planeHit.dist < meshHit.dist) {
        color = gatherLight(ray, planeHit, spheres, numSpheres,
                            lights, numLights, materials);
    } else if (sphereHit.dist < meshHit.dist) {
        color = gatherLight(ray, sphereHit, spheres, numSpheres,
                            lights, numLights, materials);
    } else {
        color = gatherLight(ray, meshHit, spheres, numSpheres,
                            lights, numLights, materials);
    }

    //if (intersectAABB(ray, aabbs[0])) {
    //    color = (float3)(0.0f, 0.0f, 0.0f);
    //} 
    write_imagef(img, coord, (float4)(color, 1.0f));
}
