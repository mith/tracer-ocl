struct Ray {
    float3 origin;
    float3 direction;
};

struct RayHit {
    float3 location;
    float3 normal;
    float dist;
    int material;
    int object;
};

struct Plane {
    float3 normal;
    float offset;
    int material;
};

struct Sphere {
    float3 center;
    float radius;
    int material;
};

struct Light {
    float3 color;
    float3 location;
    float radius;
};

struct Material {
    float3 color;
    float fresnel0;
    float roughness;
};

struct Ray createRay(int2 coord);

float3 rayPoint(struct Ray ray, float t);

struct RayHit traceRayAgainstPlanes(struct Ray ray, 
 global const struct Plane* planes, 
 int numPlanes);

float lengthSquared(float3 a);

struct RayHit traceRayAgainstSpheres(struct Ray ray,
 global const struct Sphere* spheres,
 int numSpheres); 

bool hitTestSpheres(struct Ray ray,
 float targetDistance,
 global const struct Sphere* spheres,
 int numSpheres);

float3 blinnPhong(struct Ray ray,
 struct RayHit hit,
 global const struct Light* light,
 global const struct Material* material
 );

float3 shading(struct Ray ray,
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
        global const struct Material* materials);
