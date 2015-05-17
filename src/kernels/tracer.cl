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

    global const struct Mesh* mesh = &meshes[numMesh];
    for (int p = 0; p < mesh->num_triangles; p++) {
        struct Triangle triangle = constructTriangle(vertices, vertexAttributes,
                                                     indices, p, *mesh);
        float3 uvt = intersectTriangle(ray, triangle);

        if (nearestHit.dist > uvt.z && uvt.z > 0.0f) {
            float3 loc = rayPoint(ray, uvt.z);
            float2 uv = uvt.xy;
            float3 normal = uv.x * triangle.ba->normal 
                          + uv.y * triangle.ca->normal
                          + (1.0f - uv.x - uv.y) * triangle.aa->normal;
            nearestHit.dist = uvt.z;
            nearestHit.location = loc;
            nearestHit.normal = normalize(normal);
            nearestHit.material = mesh->material;
            nearestHit.mesh = mesh;
            nearestHit.indice = &indices[p];
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
    struct RayHit hit = traceRayAgainstBVH(ray, bvh, numBVHNodes, vertices, 
                                               vertexAttributes, indices, meshes, numMeshes);

    const struct Geometry geometry = {
        vertices,
        vertexAttributes,
        indices,
        meshes,
        numMeshes,
        bvh,
        numBVHNodes
    };

    float3 color = (float3)(0.0f, 0.0f, 0.0f);
    if (hit.dist > (float)(-INFINITY) && hit.dist < (float)INFINITY) {
        color = (float3)(0.4f, 0.4f, 0.4);
        //color = gatherLight(ray, hit, &geometry,
        //                    lights, numLights, materials);
    }

    write_imagef(img, coord, (float4)(color, 1.0f));
}
