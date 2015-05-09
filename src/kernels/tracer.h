/////

#include "primitives.h"

struct Ray createCameraRay(int2 coord);
float3 rayPoint(struct Ray ray, float t);
struct RayHit traceRayAgainstPlanes(struct Ray ray,
                                    global const struct Plane* planes,
                                    int numPlanes);
float lengthSquared(float3 a);
float3 reflect(float3 v, float3 n);
struct RayHit traceRayAgainstSpheres(struct Ray ray,
                                     global const struct Sphere* spheres,
                                     int numSpheres);
struct Triangle constructTriangle(global const struct Vertex* vertices,
                                  global const struct Indices* indices,
                                  int numTriangle,
                                  float3 translate,
                                  float3 scale);
struct RayHit traceRayAgainstMeshes(struct Ray ray,
                                    global const struct Vertex* vertices,
                                    global const struct Indices* indices,
                                    global const struct Mesh* meshes,
                                    int numMeshes);
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
                   global const struct AABB* aabbs,
                   int numAABBs,
                   global const struct Material* materials);
