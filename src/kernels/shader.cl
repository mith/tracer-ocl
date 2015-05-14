////

#include "shader.h"
#include "brdf.h"
#include "intersect.h"

float3 shade(float3 normal, float3 view,
             float3 lightDir, float3 halfVec,
             float3 lightColor, float3 diffuse,
             float roughness, float fresnel0)
{
    float NdotL = dot(normal, lightDir);
    float NdotV = dot(normal, view);
    float NdotL_clamped = fmax(NdotL, 0.0f);
    float NdotV_clamped = fmax(NdotV, 0.0f);
    
    float brdf_spec = fresnel(fresnel0, halfVec, lightDir)
    * geometry(normal, halfVec, view, lightDir, roughness)
    * distribution(normal, halfVec, roughness)
    / (4.0f * NdotL_clamped * NdotV_clamped);
    float3 color_spec = NdotL_clamped * brdf_spec * lightColor;
    float3 color_diff = NdotL_clamped
    * diffuse_energy_ratio(fresnel0, normal, lightDir)
    * diffuse * lightColor;
    float3 color_add = (float3)(color_diff + color_spec);
    return clamp(color_add, 0.0f, 1.0f);
}

float3 gatherLight(struct Ray ray,
                   struct RayHit hit,
                   const struct Geometry* geometry,
                   global const struct Light* lights,
                   int numLights,
                   global const struct Material* materials)
{
    struct Material material = materials[hit.material];
    float3 view = -normalize(hit.location);
    float3 normal = hit.normal;
    float3 color = material.color / 7.0f;
    float roughness = material.roughness;
    float fresnel0 = material.fresnel0;
    
    for (int l = 0; l < numLights; l++) {
        struct Light light = lights[l];
        float3 lightDir = normalize(light.location - hit.location);
        struct Ray rayToLight = createRay(hit.location,
                                          lightDir);
        if (!occluded(rayToLight,
                            distance(hit.location,light.location),
                            hit.object,
                            geometry)) {
            float3 halfVec = normalize(lightDir + view);
            float lightDist = distance(hit.location, light.location);
            float att = clamp(1.0f - lightDist * lightDist
                              / (light.radius * light.radius), 0.0f, 1.0f);
            att *= att;
            color += att * shade(normal, view, lightDir, halfVec,
                                 light.color, material.color, roughness, fresnel0);
        }
    }
    
    return color;
}

bool occluded(struct Ray ray,
              float targetDistance,
              global const void* ignoredObject,
              const struct Geometry* geometry)
{
    for (int s = 0; s < geometry->numSpheres; s++) {
        if (&geometry->spheres[s] == ignoredObject)
            continue;

        struct Sphere sphere = geometry->spheres[s];
        float t = intersectSphere(ray, sphere);
        if (t < targetDistance && t > 0.0f) {
            return true;
        }
    }

    for (int b = 0; b < geometry->numBVHNodes; b++) {
        struct BVHNode bvhnode = geometry->bvh[b];
        bvhnode.bounds.min = bvhnode.bounds.min
                           * bvhnode.scale
                           + bvhnode.position;
        bvhnode.bounds.max = bvhnode.bounds.max
                           * bvhnode.scale
                           + bvhnode.position;
        if (intersectAABB(ray, bvhnode.bounds)) {
     
            struct Mesh mesh = geometry->meshes[b];
            for (int p = 0; p < mesh.num_triangles; p++) {
                if (&geometry->indices[p] == ignoredObject)
                    continue;

                struct Triangle triangle = constructTriangle(geometry->vertices,
                                                             geometry->indices,
                                                             p, mesh);
                float t = intersectTriangle(ray, triangle);
                if (t < targetDistance && t > 0.0f) {
                    return true;
                }
            }
        }
    }

    return false;
}
