/////

#include "primitives.h"

struct Ray createCameraRay(int2 coord);
float3 rayPoint(struct Ray ray, float t);
float lengthSquared(float3 a);
float3 reflect(float3 v, float3 n);
float3 barycentric(float3 loc, struct Triangle triangle);
struct RayHit traceRayAgainstMesh(struct Ray ray,
                                  global const struct Vertex* vertices,
                                  global const struct VertexAttributes* vertexAttributes,
                                  global const struct Indices* indices,
                                  global const struct Mesh* meshes,
                                  int numMesh);
struct RayHit traceRayAgainstBVH(struct Ray ray,
                                 global const struct BVHNode* bvh,
                                 int numBVHNodes,
                                 global const struct Vertex* vertices,
                                 global const struct VertexAttributes* vertexAttributes,
                                 global const struct Indices* indices,
                                 global const struct Mesh* meshes,
                                 int numMeshes);
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
                   global const struct Material* materials);
