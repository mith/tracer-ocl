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

#include "cl.hpp"
#include <vector>
#include <array>
#include <glm/gtc/quaternion.hpp>

struct Light {
    cl_float3 color;
    cl_float3 location;
    cl_float radius;
};

struct Material {
    cl_int diffuse;
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
    cl_float2 texcoord;

    VertexAttributes (std::array<float, 3> & n,
                      std::array<float, 2> & t)
        : normal{{n[0], n[1], n[2]}}
        , texcoord{{t[0], t[1]}}
    {}
};

typedef cl_uint Indice;

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
    glm::quat orientation;
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
