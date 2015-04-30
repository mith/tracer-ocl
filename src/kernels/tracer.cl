#include "tracer.h"

#include "intersect.h"

#include "brdf.h"

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

struct RayHit traceRayAgainstPlanes(struct Ray ray, 
        global const struct Plane* planes, 
        int numPlanes) 
{
    struct RayHit nearestHit; 
    nearestHit.dist = (INFINITY, INFINITY, INFINITY);
    nearestHit.material = -1;

    for (int p = 0; p < numPlanes; p++) {
        struct Plane plane = planes[p];

        float t = intersectPlane(ray, plane);

        if (t < 0.0001f) {
            continue;
        }

        float3 loc = rayPoint(ray, t);
        float dist = distance((float3)(0.0f, 0.0f, 0.0f), loc);

        if (nearestHit.dist > dist) {
            nearestHit.dist = dist;
            nearestHit.location = loc;
            nearestHit.normal = plane.normal;
            nearestHit.material = plane.material;
            nearestHit.object = p;
        }
    }

    return nearestHit;
}

float lengthSquared(float3 a) 
{
    return a.x * a.x + a.y * a.y + a.z * a.z;
}

float3 reflect(float3 i, float3 n)
{
    return i - 2.0f * n * dot(n, i);
}

struct RayHit traceRayAgainstSpheres(struct Ray ray,
        global const struct Sphere* spheres,
        int numSpheres) 
{
    struct RayHit nearestHit; 
    nearestHit.dist = (INFINITY, INFINITY, INFINITY);
    nearestHit.material = -1;

    for (int s = 0; s < numSpheres; s++) {
        struct Sphere sphere = spheres[s];
        float t = intersectSphere(ray, sphere);
        float3 loc = rayPoint(ray, t);
        float3 normal = normalize(loc - sphere.center);

        float dist = distance(loc, (float3)(0.0f, 0.0f, 0.0f));

        if (nearestHit.dist > dist) {
            nearestHit.dist = dist;
            nearestHit.location = loc;
            nearestHit.normal = normal;
            nearestHit.material = sphere.material;
            nearestHit.object = s;
        }
    }

    return nearestHit;
}

bool hitTestSpheres(struct Ray ray,
        float targetDistance,
        global const struct Sphere* spheres,
        int numSpheres) 
{
    float nearestDist = targetDistance - 0.0001f;
    int object = -1;

    for (int s = 0; s < numSpheres; s++) {
        struct Sphere sphere = spheres[s];
        float t = intersectSphere(ray, sphere);
        if (-t < nearestDist && -t > 0.0f) {
            nearestDist = t;
            object = s;
        }
    }
    return object > -1;
}

// float3 shade(float3 normal, float3 view, 
//         float3 lightDir, float3 halfVec,
//         float3 lightColor, float3 diffuse, 
//         float roughness, float fresnel0)
// {
//     float NdotL = dot(normal, lightDir);
//     float NdotV = dot(normal, view);
//     float NdotL_clamped = fmax(NdotL, 0.0f);
//     float NdotV_clamped = fmax(NdotV, 0.0f);
// 
//     float brdf_spec = fresnel(fresnel0, halfVec, lightDir)
//         * geometry(normal, halfVec, view, lightDir, roughness)
//         * distribution(normal, halfVec, roughness)
//         / (4.0f * NdotL_clamped * NdotV_clamped);
//     float3 color_spec = NdotL_clamped * brdf_spec * lightColor;
//     float3 color_diff = NdotL_clamped 
//         * diffuse_energy_ratio(fresnel0, normal, lightDir)
//         * diffuse * lightColor;
//     float3 color_add = (float3)(color_diff + color_spec);
//     return clamp(color_add, 0.0f, 1.0f);
// 
// }
// 
// float3 gatherLight(struct Ray ray,
//         struct RayHit hit,
//         global const struct Sphere* spheres,
//         int numSpheres,
//         global const struct Light* lights,
//         int numLights,
//         global const struct Material* materials) 
// {
//     struct Material material = materials[hit.material];
//     float3 view = -normalize(hit.location);
//     float3 normal = hit.normal;
//     float3 color = material.color / 7.0f;
//     float roughness = material.roughness;
//     float fresnel0 = material.fresnel0;
// 
//     for (int l = 0; l < numLights; l++) {
//         struct Light light = lights[l];
//         float3 lightDir = normalize(light.location - hit.location);
//         struct Ray rayToLight;
//         rayToLight.origin = light.location;
//         rayToLight.direction = lightDir;
//         if (!hitTestSpheres(rayToLight, 
//                     distance(hit.location, light.location), 
//                     spheres, numSpheres)) {
//             float3 halfVec = normalize(lightDir + view);
//             float lightDist = distance(hit.location, light.location);
//             float att = clamp(1.0f - lightDist * lightDist 
//                     / (light.radius * light.radius), 0.0f, 1.0f);
//             att *= att;
//             color += att * shade(normal, view, lightDir, halfVec, 
//                     light.color, material.color, roughness, fresnel0);
//         }
//     }
// 
//     return color;
// }

void __kernel tracer(write_only image2d_t img, 
        __global const struct Light* lights,
        int numLights,
        __global const struct Plane* planes,
        int numPlanes,
        __global const struct Sphere* spheres,
        int numSpheres,
        __global const struct Triangle* triangles,
        int numTriangles,
        __global const struct Material* materials) 
{
    const int2 coord = (int2)(get_global_id(0), get_global_id(1));
    struct Ray ray = createCameraRay(coord);
    struct RayHit planeHit = traceRayAgainstPlanes(ray, planes, numPlanes);
    struct RayHit sphereHit = traceRayAgainstSpheres(ray, spheres, numSpheres);
    float3 color = (float3)(0.0f, 0.0f, 0.0f);
    if (planeHit.dist < sphereHit.dist) {
        color = planeHit.location; 
        // gatherLight(ray, planeHit, spheres, numSpheres, lights, numLights, materials);
    } else {
        color = sphereHit.location;
        //gatherLight(ray, sphereHit, spheres, numSpheres, lights, numLights, materials);
    }
    write_imagef(img, coord, (float4)(color, 1.0f));
}
