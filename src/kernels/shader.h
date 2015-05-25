#ifndef SHADER_H_
#define SHADER_H_

#include "primitives.h"

float3 shade(float3 normal, float3 view,
             float3 lightDir, float3 halfVec,
             float3 lightColor, float3 diffuse,
             float roughness, float fresnel0);
float3 gatherLight(struct Ray ray,
                   struct RayHit hit,
                   const struct Geometry* geometry,
                   global const struct Light* lights,
                   int numLights,
                   global const struct Material* materials,
                   read_only image2d_array_t diffuse);
bool occluded(struct Ray ray,
              float targetDistance,
              global const Indice* ignoredIndices,
              const struct Geometry* geometry);
#endif
