//////

#ifndef PRIMITIVES_H_
#define PRIMITIVES_H_

struct Ray {
    float3 origin;
    float3 direction;
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

struct Triangle {
    float3 a;
    float3 b;
    float3 c;
    int material;
};

struct RayHit {
    float3 location;
    float3 normal;
    float dist;
    int material;
    global const void* object;
};

// struct RayHit {
//     float3 normal;
//     float3 incidence;
//     float3 location;
//     int material;
// };

#endif
