#include "tracer.h"
#include "brdf.h"

struct Ray createRay(int2 coord) 
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

        float t = -(dot(plane.normal, ray.origin) + plane.offset)
            / dot(plane.normal, ray.direction);

        if (t < 0.0f) {
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

struct RayHit traceRayAgainstSpheres(struct Ray ray,
        global const struct Sphere* spheres,
        int numSpheres) 
{
    struct RayHit nearestHit; 
    nearestHit.dist = (INFINITY, INFINITY, INFINITY);
    nearestHit.material = -1;

    for (int s = 0; s < numSpheres; s++) {
        struct Sphere sphere = spheres[s];

        float b = dot(ray.direction, -sphere.center);
        float c = lengthSquared(sphere.center) - sphere.radius * sphere.radius;
        float d = b * b -c;
        if (d < 0.0f) {
            continue;
        }

        float t = -b - sqrt(d);
        float3 loc = rayPoint(ray, t);
        float3 normal = normalize(loc - sphere.center);

        float dist = distance(loc, (float3)(0.0, 0.0, 0.0));

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

        float3 oc = ray.origin - sphere.center;
        float b = dot(ray.direction, oc);
        float c = dot(oc, oc) - sphere.radius * sphere.radius;
        float d = b * b - c;

        if (d < 0.0f) {
            continue;
        }

        float t = -b - sqrt(d);
        if (-t < nearestDist && -t > 0.0f) {
            nearestDist = t;
            object = s;
        }
    }
    return object > -1;
}

float3 shading(struct Ray ray,
        struct RayHit hit,
        global const struct Sphere* spheres,
        int numSpheres,
        global const struct Light* lights,
        int numLights,
        global const struct Material* materials) 
{
    struct Material material = materials[hit.material];
    float3 color = material.color / 7;
    float3 normal = hit.normal;
    float roughness = material.roughness;
    float fresnel0 = material.fresnel0;

    for (int l = 0; l < numLights; l++) {
        struct Light light = lights[l];
        float3 lightDir = normalize(light.location - hit.location);
        struct Ray rayToLight;
        rayToLight.origin = light.location;
        rayToLight.direction = lightDir;
        if (!hitTestSpheres(rayToLight, 
                    distance(hit.location, light.location), 
                    spheres, numSpheres)) {
            float3 view = -normalize(hit.location);
            float3 halfVec = normalize(lightDir + view);

            float NdotL = dot(normal, lightDir);
            float NdotV = dot(normal, view);
            float NdotL_clamped = fmax(NdotL, 0.0f);
            float NdotV_clamped = fmax(NdotV, 0.0f);

            float brdf_spec = fresnel(material.fresnel0, halfVec, lightDir)
                * geometry(normal, halfVec, view, lightDir, roughness)
                * distribution(normal, halfVec, roughness)
                / (4.0f * NdotL_clamped * NdotV_clamped);
            float3 color_spec = NdotL_clamped * brdf_spec * light.color;
            float3 color_diff = NdotL_clamped 
                * diffuse_energy_ratio(fresnel0, normal, lightDir)
                * material.color * light.color;
            float lightDist = fast_distance(light.location, hit.location);
            float att = clamp(1.0f - lightDist * lightDist 
                             / (light.radius * light.radius), 0.0f, 1.0f);
            att *= att;
            float3 color_add = (float3)(color_diff + color_spec);
            color += clamp(att * color_add, 0.0f, 1.0f);
        }
    }
    return color;
}

void kernel tracer(write_only image2d_t img, 
        global const struct Light* lights,
        int numLights,
        global const struct Plane* planes,
        int numPlanes,
        global const struct Sphere* spheres,
        int numSpheres,
        global const struct Material* materials) {
    const int2 coord = (int2)(get_global_id(0), get_global_id(1));
    struct Ray ray = createRay(coord);
    struct RayHit planeHit = traceRayAgainstPlanes(ray, planes, numPlanes);
    struct RayHit sphereHit = traceRayAgainstSpheres(ray, spheres, numSpheres);
    float3 color = (float3)(0.1, 0.1, 0.1);
    if (planeHit.dist < sphereHit.dist) {
        color = shading(ray, planeHit, spheres, numSpheres, lights, numLights, materials);
    } else {
        color = shading(ray, sphereHit, spheres, numSpheres, lights, numLights, materials);
    }
    write_imagef(img, coord, (float4)(color, 1.0));
}
