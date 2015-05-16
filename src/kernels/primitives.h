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
    float3 position;
    float3 scale;

    int num_triangles;
    int material;

    int base_vertex;
    int base_triangle;
};

struct Vertex {
    float3 position;
};

struct VertexAttributes {
    float3 normal;
    float2 texcoord;
};

struct Indices {
    uint3 vertex;
};

struct Triangle {
    struct Vertex a;
    global const struct VertexAttributes* aa;
    struct Vertex b;
    global const struct VertexAttributes* ba;
    struct Vertex c;
    global const struct VertexAttributes* ca;
};

struct AABB {
    float3 min;
    float3 max;
};

struct BVHNode {
    struct AABB bounds;
    float3 position;
    float3 scale;
    int mesh;
};

struct Geometry {
    global const struct Sphere* spheres;
    int numSpheres;
    global const struct Plane* planes;
    int numPlanes;
    global const struct Vertex* vertices;
    global const struct VertexAttributes* vertexAttributes;
    global const struct Indices* indices;
    global const struct Mesh* meshes;
    int numMeshes;
    global const struct BVHNode* bvh;
    int numBVHNodes;
};

struct Triangle constructTriangle(global const struct Vertex* vertices,
                                  global const struct VertexAttributes* vertexAttributes,
                                  global const struct Indices* indices,
                                  int numTriangle,
                                  struct Mesh);

struct Ray createRay(float3 origin, float3 direction);

#endif
