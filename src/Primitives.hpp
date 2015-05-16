#pragma once

#ifdef __APPLE__
#include <OpenCL/cl.h>
#include <OpenCL/cl_gl.h>
#include <OpenCL/cl_gl_ext.h>
#include <OpenCL/cl_platform.h>
#elif defined __linux__
#include <CL/cl.h>
#include <CL/cl_gl.h>
#include <CL/cl_gl_ext.h>
#include <CL/cl_platform.h>
#endif

#include <vector>
#include <array>

struct Plane {
    cl_float3 normal;
    cl_float offset;
    cl_int material;
};

struct Sphere {
    cl_float3 center;
    cl_float radius;
    cl_int material;
};

struct Light {
    cl_float3 color;
    cl_float3 location;
    cl_float radius;
};

struct Material {
    cl_float3 color;
    cl_float fresnel0;
    cl_float roughness;
};

struct Vertex {
    cl_float3 position;

    Vertex (std::array<float, 3> & p)
        : position{{p[0], p[1], p[2]}}
    {}
};

struct VertexAttributes {
    cl_float3 normal;
    cl_float2 uv;

    VertexAttributes (std::array<float, 3> & n)
        : normal{{n[0], n[1], n[2]}}
        , uv{{0.0f, 0.0f}}
    {}
};

struct Indice {
    cl_uint3 i;

    Indice (std::array<unsigned int, 3> i)
        : i{{i[0], i[1], i[2]}} {}
};

struct AABB {
    cl_float3 min;
    cl_float3 max;
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<VertexAttributes> vertexAttributes;
    std::vector<Indice> indices;
    AABB bounds;
};

struct CLMesh {
    cl_float3 position;
    cl_float3 scale;

    cl_int num_triangles;
    cl_int material;

    cl_int base_vertex;
    cl_int base_triangle;
};

struct BVHNode {
    AABB bounds;
    cl_float3 position;
    cl_float3 scale;
    cl_int mesh;
};
