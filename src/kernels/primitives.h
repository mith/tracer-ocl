//////

#ifndef PRIMITIVES_H_
#define PRIMITIVES_H_

struct Ray {
    float3 origin;
    float3 direction;
};

struct Plane {
    float3 normal;
    float3 offset;
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
    float3 points[3];
};

#endif