/////

#include "tracer.h"
#include "intersect.h"
#include "shader.h"

struct Ray createCameraRay(int2 coord)
{
    float width = (float)get_global_size(0);
    float height = (float)get_global_size(1);

    float ratio = height / width;

    const float x_ratio = 1.0f / (float)get_global_size(0);
    const float y_ratio = ratio / (float)get_global_size(1);

    float3 direction = normalize((float3)(coord.x * x_ratio - 0.5f,
                                       coord.y * y_ratio - ratio / 2,
                                       -0.3f));
    return createRay((float3)(0.0f, 0.0f, 0.0f),
                     direction);
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

float3 barycentric(float3 loc, struct Triangle triangle)
{
    float3 v0 = triangle.b.position - triangle.a.position;
    float3 v1 = triangle.c.position - triangle.a.position;
    float3 v2 = loc - triangle.a.position;

    float d00 = dot(v0, v0);
    float d01 = dot(v0, v1);
    float d11 = dot(v1, v1);
    float d20 = dot(v2, v0);
    float d21 = dot(v2, v1);
    float invDenom = 1.0f / (d00 * d11 - d01 * d01);
    float v = (d11 * d20 - d01 * d21) * invDenom;
    float w = (d00 * d21 - d01 * d20) * invDenom;
    float u = 1.0f - v - w;
    return (float3)(u, v, w);
}

struct RayHit traceRayAgainstMesh(struct Ray ray,
                                  global const struct Vertex* vertices,
                                  global const struct VertexAttributes* vertexAttributes,
                                  global const struct Indices* indices,
                                  global const struct Mesh* meshes,
                                  int numMesh)
{
    struct RayHit nearestHit;
    nearestHit.dist = (float)(INFINITY);

    struct Mesh mesh = meshes[numMesh];
    for (int p = 0; p < mesh.num_triangles; p++) {
        struct Triangle triangle = constructTriangle(vertices, vertexAttributes,
                                                     indices, p, mesh);
        float t = intersectTriangle(ray, triangle);

        if (nearestHit.dist > t && t > 0.0f) {
            float3 loc = rayPoint(ray, t);
            float3 bc = barycentric(loc, triangle);
            float3 normal = bc.x * triangle.aa->normal 
                          + bc.y * triangle.ba->normal
                          + bc.z * triangle.ca->normal;
            nearestHit.dist = t;
            nearestHit.location = loc;
            nearestHit.normal = normalize(normal);
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
                                 global const struct VertexAttributes* vertexAttributes,
                                 global const struct Indices* indices,
                                 global const struct Mesh* meshes,
                                 int numMeshes)
{
    struct RayHit nearestHit;
    nearestHit.dist = (float)(INFINITY);
    for (int b = 0; b < numBVHNodes; b++) {
        struct BVHNode bvhnode = bvh[b];
        bvhnode.bounds.min = bvhnode.bounds.min
                           * bvhnode.scale
                           + bvhnode.position;
        bvhnode.bounds.max = bvhnode.bounds.max
                           * bvhnode.scale
                           + bvhnode.position;
        if (intersectAABB(ray, bvhnode.bounds)) {
            struct RayHit hit = traceRayAgainstMesh(ray, vertices, vertexAttributes,
                                                    indices, meshes, bvhnode.mesh);
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
                   global const struct VertexAttributes* vertexAttributes,
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
                                               vertexAttributes, indices, meshes, numMeshes);

    const struct Geometry geometry = {
        spheres,
        numSpheres,
        planes,
        numPlanes,
        vertices,
        vertexAttributes,
        indices,
        meshes,
        numMeshes,
        bvh,
        numBVHNodes
    };

    float3 color = (float3)(0.0f, 0.0f, 0.0f);
    if (planeHit.dist < sphereHit.dist && planeHit.dist < meshHit.dist) {
        color = gatherLight(ray, planeHit, &geometry,
                            lights, numLights, materials);
    } else if (sphereHit.dist < meshHit.dist) {
        color = gatherLight(ray, sphereHit, &geometry,
                            lights, numLights, materials);
    } else {
        color = gatherLight(ray, meshHit, &geometry,
                            lights, numLights, materials);
    }

    write_imagef(img, coord, (float4)(color, 1.0f));
}
