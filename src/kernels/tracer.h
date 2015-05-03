/////

#include "primitives.h"

struct RayHit {
    float3 location;
    float3 normal;
    float dist;
    int material;
    int object;
};

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
bool hitTestSpheres(struct Ray ray,
                    float targetDistance,
                    global const struct Sphere* spheres,
                    int numSpheres);
float3 shade(float3 normal, float3 view,
             float3 lightDir, float3 halfVec,
             float3 lightColor, float3 diffuse,
             float roughness, float fresnel0);
float3 gatherLight(struct Ray ray,
                   struct RayHit hit,
                   global const struct Sphere* spheres,
                   int numSpheres,
                   global const struct Light* lights,
                   int numLights,
                   global const struct Material* materials);
void kernel tracer(write_only image2d_t img,
                   global const struct Light* lights,
                   int numLights,
                   global const struct Plane* planes,
                   int numPlanes,
                   global const struct Sphere* spheres,
                   int numSpheres,
                   global const struct Triangle* triangles,
                   int numTriangles,
                   global const struct Material* materials);