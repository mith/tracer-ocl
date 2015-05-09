//////

#ifndef PRIMITIVES_H_
#define PRIMITIVES_H_

struct Ray {
    float3 origin;
    float3 direction;
    float3 direction_inverse;
};

struct RayHit {
    float3 location;
    float3 normal;
    float dist;
    int material;
    global const void* object;
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

struct Mesh {
    int num_triangles;
    int material;

    float3 translate;
    float3 scale;
};

struct Vertex {
    float3 v;
};

struct Indices {
    uint3 t;
};

struct Triangle {
    float3 a;
    float3 b;
    float3 c;
};

struct AABB {
    float3 min;
    float3 max;
};

#endif
